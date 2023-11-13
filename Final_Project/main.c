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
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
// Include custom libraries
#include "vga_graphics.h"
#include "pt_cornell_rp2040_v1.h"


// User input thread
static PT_THREAD (protothread_serial(struct pt *pt``))
{
    PT_BEGIN(pt) ;
    static unsigned int test_in = 0 ;
    while(1) {
        sprintf(pt_serial_out_buffer, "Enter an color in integer form (in range 0-65536): \r\n");
        serial_write ;
        serial_read ;
        sscanf(pt_serial_in_buffer,"%d", &test_in) ;
        if ( test_in <= 65536 ) {
            printf("%d", test_in);
        }
        test_in = 0;
    }
    PT_END(pt) ;
}


int main() {

    // Initialize stdio
    stdio_init_all();

    // Initialize VGA
    initVGA() ;

    // gpio_init(11);
    // gpio_pull_up(11);
    // gpio_set_input_enabled(11, 1);

    pt_add_thread(protothread_serial) ;
    pt_schedule_start ;

}
