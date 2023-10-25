/**
 * V. Hunter Adams (vha3@cornell.edu)
 * PWM demo code with serial input
 * 
 * This demonstration sets a PWM duty cycle to a
 * user-specified value.
 * 
 * HARDWARE CONNECTIONS
 *   - GPIO 5 ---> PWM output
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "hardware/pwm.h"
#include "hardware/irq.h"

// Include hardware libraries
#include "hardware/pwm.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
// Include custom libraries
#include "vga_graphics.h"
#include "mpu6050.h"
#include "pt_cornell_rp2040_v1.h"

// Arrays in which raw measurements will be stored
fix15 acceleration[3], gyro[3];

// character array
char screentext[40];

// draw speed
int threshold = 10 ;

// Some macros for max/min/abs
#define min(a,b) ((a<b) ? a:b)
#define max(a,b) ((a<b) ? b:a)
#define abs(a) ((a>0) ? a:-a)

// semaphore
static struct pt_sem vga_semaphore ;

// PWM wrap value and clock divide value
// For a CPU rate of 125 MHz, this gives
// a PWM frequency of 1 kHz.
#define WRAPVAL 5000
#define CLKDIV 25.0f

// Variable to hold PWM slice number
uint slice_num ;

// PWM duty cycle
volatile int control  = 1000;
volatile int old_control = 0 ;
volatile int low_pass = 0 ;

volatile fix15 complementary_angle = int2fix15(90);
volatile fix15 prev_complementary_angle = int2fix15(90);
fix15 PI = float2fix15(3.15149);

volatile fix15 filtered_ay = int2fix15(0);
volatile fix15 filtered_az = int2fix15(1);

volatile int desired_angle = 86;
volatile fix15 error_ang = int2fix15(0);
volatile fix15 last_err = int2fix15(0);

fix15 Kt = float2fix15(0.0000000005);

fix15 Kp = int2fix15(20);
fix15 Kd = int2fix15(2000);
fix15 Ki = float2fix15(0.03125);

volatile fix15 proportional_cntl = int2fix15(0);
volatile fix15 differential_cntl = int2fix15(0);
volatile fix15 integral_cntl = int2fix15(0);
fix15 motor_lp = float2fix15(0.95);
fix15 dd = float2fix15(0.005);


int oppositeSigns(fix15 x, fix15 y) {
    if ( (fix2int15(x) ^ fix2int15(y)) < 0 ) {
        return 1; // True
    } return 0; // False
}


volatile int ii = 0;
// PWM interrupt service routine
void on_pwm_wrap() {
    // Clear the interrupt flag that brought us here
    pwm_clear_irq(pwm_gpio_to_slice_num(5));
    // Update duty cycle
    if (control!=old_control) {
        old_control = control ;
        pwm_set_chan_level(slice_num, PWM_CHAN_B, control);
    }
    mpu6050_read_raw(acceleration, gyro);

    // Accelerometer angle (degrees - 15.16 fixed point) 
    // NO SMALL ANGLE APPROXIMATION
    filtered_ay = filtered_ay + ((acceleration[1] - filtered_ay)>>4) ;
    filtered_az = filtered_az + ((acceleration[2] - filtered_az)>>4) ;
    fix15 accel_angle = multfix15(float2fix15(atan2(-filtered_ay, filtered_az)) + PI, oneeightyoverpi);

    low_pass = low_pass + ((control - low_pass)>>4);

    // Gyro angle delta (measurement times timestep) (15.16 fixed point)
    fix15 gyro_angle_delta = multfix15(gyro[0], zeropt001) ;

    // Complementary angle (degrees - 15.16 fixed point)
    complementary_angle = multfix15(complementary_angle - gyro_angle_delta, zeropt999) + multfix15(accel_angle, zeropt001);

    // int curr_control = control;
    // float error = desired_angle - fix2float15(complementary_angle);
    
    // curr_control += error / 50.0;

    // curr_control = max(min(5000, curr_control), 0);
    // control = curr_control;

    int temp_ctrl = 0;
    // error angle = desired_angle - previous complementary_angle
    error_ang = int2fix15(desired_angle) - complementary_angle;

    // ++ii;
    // ii = (ii >= 100) ? 0 : ii;
    // if (ii == 0) {
    //     printf("comp: %f, desired: %d\n", fix2float15(complementary_angle), desired_angle);
    // }
    
    proportional_cntl = multfix15(Kp, error_ang);
    differential_cntl = multfix15(Kd, error_ang - last_err);
    integral_cntl += error_ang;

    // integral ctrl is accumulator
    // set to 0 if sign of errors different
    if (oppositeSigns(error_ang, last_err) == 1) {
        integral_cntl = 0;
    }

    integral_cntl = multfix15(Ki, integral_cntl);

    // add 3 controls together
    temp_ctrl = fix2int15(proportional_cntl) + fix2int15(differential_cntl) + fix2int15(integral_cntl);
    // temp_ctrl = fix2int15(proportional_cntl);
    // clamp control to between 0 and 5k
    temp_ctrl = min(max(0, temp_ctrl), 5000);
    // set control to the new control
    control = temp_ctrl;
    last_err = error_ang;

    // Signal VGA to draw
    PT_SEM_SIGNAL(pt, &vga_semaphore);
}

// Thread that draws to VGA display
static PT_THREAD (protothread_vga(struct pt *pt))
{
    // Indicate start of thread
    PT_BEGIN(pt) ;

        // We will start drawing at column 81
    static int xcoord = 81 ;
    
    // Rescale the measurements for display
    static float OldRange = 500. ; // (+/- 250)
    static float NewRange = 150. ; // (looks nice on VGA)
    static float OldMin = -250. ;
    static float OldMax = 250. ;

    // Control rate of drawing
    static int throttle ;

    // Draw the static aspects of the display
    setTextSize(1) ;
    setTextColor(WHITE);

    // Draw bottom plot
    drawHLine(75, 430, 5, CYAN) ;
    drawHLine(75, 355, 5, CYAN) ;
    drawHLine(75, 280, 5, CYAN) ;
    drawVLine(80, 280, 150, CYAN) ;
    sprintf(screentext, "2500") ;
    setCursor(50, 350) ;
    writeString(screentext) ;
    sprintf(screentext, "5000") ;
    setCursor(50, 280) ;
    writeString(screentext) ;
    sprintf(screentext, "0") ;
    setCursor(50, 425) ;
    writeString(screentext) ;

    // Draw top plot
    drawHLine(75, 230, 5, CYAN) ;
    drawHLine(75, 155, 5, CYAN) ;
    drawHLine(75, 80, 5, CYAN) ;
    drawVLine(80, 80, 150, CYAN) ;
    sprintf(screentext, "90") ;
    setCursor(50, 150) ;
    writeString(screentext) ;
    sprintf(screentext, "180") ;
    setCursor(45, 75) ;
    writeString(screentext) ;
    sprintf(screentext, "0") ;
    setCursor(45, 225) ;
    writeString(screentext) ;
    
    while (true) {
        // Wait on semaphore
        PT_SEM_WAIT(pt, &vga_semaphore);
        // Increment drawspeed controller
        throttle += 1 ;
        // If the controller has exceeded a threshold, draw
        if (throttle >= threshold) { 
            // Zero drawspeed controller
            throttle = 0 ;

            // Erase a column
            drawVLine(xcoord, 0, 480, BLACK) ;

            // Draw bottom plot (multiply by 120 to scale from +/-2 to +/-250)
            drawPixel(xcoord, 431 - (low_pass * 0.03), WHITE) ;
            // printf("%d\n", (int)((fix2float15(low_pass))));

            // Draw top plot
            drawPixel(xcoord, 225 - (int)((fix2float15(complementary_angle) - 90.0)*0.8333), WHITE) ;

            // Update horizontal cursor
            if (xcoord < 609) {
                xcoord += 1 ;
            }
            else {
                xcoord = 81 ;
            }
        }
    }
    // Indicate end of thread
    PT_END(pt);
}

// User input thread
static PT_THREAD (protothread_serial(struct pt *pt))
{
    PT_BEGIN(pt) ;
    static int test_in = -1 ;
    while(1) {
        sprintf(pt_serial_out_buffer, "choose 1 to change desired angle, 2 Kp, 3 Kd, 4 Ki: \r\n");
        serial_write ;
        serial_read ;
        sscanf(pt_serial_in_buffer,"%d", &test_in) ;
        if ( test_in == 1 ) {
            sprintf(pt_serial_out_buffer, "input desired angle, 0 to 180 degrees: \r\n");
            serial_write ;
            serial_read ;
            sscanf(pt_serial_in_buffer,"%d", &test_in) ;
            if ( test_in >= 0 && test_in <= 180 ) {
                desired_angle = test_in+90 ;
            }
            test_in = -1;
        }
        else if ( test_in == 2 ) {
            sprintf(pt_serial_out_buffer, "input Kp, 0 to 100: \r\n");
            serial_write ;
            serial_read ;
            sscanf(pt_serial_in_buffer,"%d", &test_in) ;
            if ( test_in >= 0 && test_in <= 100 ) {
                Kp = int2fix15(test_in) ;
            }
            test_in = -1;
        }
        else if ( test_in == 3 ) {
            sprintf(pt_serial_out_buffer, "input Kd, 0 to 1000: \r\n");
            serial_write ;
            serial_read ;
            sscanf(pt_serial_in_buffer,"%d", &test_in) ;
            if ( test_in >= 0 && test_in <= 1000 ) {
                Kd = int2fix15(test_in) ;
            }
            test_in = -1;
        }
        else if ( test_in == 4 ) {
            sprintf(pt_serial_out_buffer, "input Ki, 0 to 0.1: \r\n");
            serial_write ;
            serial_read ;
            sscanf(pt_serial_in_buffer,"%f", &test_in) ;
            if ( test_in >= 0 && test_in <= 0.1 ) {
                Ki = float2fix15(test_in) ;
            }
            test_in = -1;
        }
    }
    PT_END(pt) ;
}

// Entry point for core 1
void core1_entry() {
    pt_add_thread(protothread_vga) ;
    pt_schedule_start ;
}

int main() {

    // Initialize stdio
    stdio_init_all();

    // Initialize VGA
    initVGA() ;

    i2c_init(I2C_CHAN, I2C_BAUD_RATE) ;
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C) ;
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C) ;
    gpio_pull_up(SDA_PIN) ;
    gpio_pull_up(SCL_PIN) ;

    // MPU6050 initialization
    mpu6050_reset();
    mpu6050_read_raw(acceleration, gyro);

    // Tell GPIO 5 that it is allocated to the PWM
    gpio_set_function(5, GPIO_FUNC_PWM);
    gpio_set_function(4, GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to GPIO 5 (it's slice 2)
    slice_num = pwm_gpio_to_slice_num(5);

    // Mask our slice's IRQ output into the PWM block's single interrupt line,
    // and register our interrupt handler
    pwm_clear_irq(slice_num);
    pwm_set_irq_enabled(slice_num, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    // This section configures the period of the PWM signals
    pwm_set_wrap(slice_num, WRAPVAL) ;
    pwm_set_clkdiv(slice_num, CLKDIV) ;

    // This sets duty cycle
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 3125);

    // Start the channel
    pwm_set_mask_enabled((1u << slice_num));

    // start core 1 
    multicore_reset_core1();
    multicore_launch_core1(core1_entry);

    pt_add_thread(protothread_serial) ;
    pt_schedule_start ;

}
