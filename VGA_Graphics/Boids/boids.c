
/**
 * Hunter Adams (vha3@cornell.edu)
 * 
 * This demonstration animates two balls bouncing about the screen.
 * Through a serial interface, the user can change the ball color.
 *
 * HARDWARE CONNECTIONS
 *  - GPIO 16 ---> VGA Hsync
 *  - GPIO 17 ---> VGA Vsync
 *  - GPIO 18 ---> 330 ohm resistor ---> VGA Red
 *  - GPIO 19 ---> 330 ohm resistor ---> VGA Green
 *  - GPIO 20 ---> 330 ohm resistor ---> VGA Blue
 *  - RP2040 GND ---> VGA GND
 *
 * RESOURCES USED
 *  - PIO state machines 0, 1, and 2 on PIO instance 0
 *  - DMA channels 0, 1
 *  - 153.6 kBytes of RAM (for pixel color data)
 *
 */

// Include the VGA grahics library
#include "vga_graphics.h"
// Include standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
// Include Pico libraries
#include "pico/stdlib.h"
#include "pico/divider.h"
#include "pico/multicore.h"
// Include hardware libraries
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
// Include protothreads
#include "pt_cornell_rp2040_v1.h"

// === the fixed point macros ========================================
typedef signed int fix15 ;
#define multfix15(a,b) ((fix15)((((signed long long)(a))*((signed long long)(b)))>>15))
#define float2fix15(a) ((fix15)((a)*32768.0)) // 2^15
#define fix2float15(a) ((float)(a)/32768.0)
#define absfix15(a) abs(a) 
#define int2fix15(a) ((fix15)(a << 15))
#define fix2int15(a) ((int)(a >> 15))
#define char2fix15(a) (fix15)(((fix15)(a)) << 15)
#define divfix(a,b) (fix15)(div_s64s64( (((signed long long)(a)) << 15), ((signed long long)(b))))
// #define sqrtfix(a) (float2fix15(sqrt(fix2float15(a)))


// Wall detection
#define hitBottom(b) (b>int2fix15(380))
#define hitTop(b) (b<int2fix15(100))
#define hitLeft(a) (a<int2fix15(100))
#define hitRight(a) (a>int2fix15(540))

fix15 turnfactor = float2fix15(0.2);
fix15 visualRange = int2fix15(40);
fix15 protectedRange = int2fix15(8);
fix15 centeringfactor = float2fix15(0.0005);
fix15 avoidfactor = float2fix15(0.05);
fix15 matchingfactor = float2fix15(0.05);
fix15 maxspeed = int2fix15(6);
fix15 minspeed = int2fix15(3);
fix15 predatorturnfactor = float2fix15(0.5);
fix15 predatorRange = int2fix15(100);

// uS per frame
#define FRAME_RATE 33000

// the color of the boid
char color = WHITE ;

// total number of boids
int numberOfBoids = 0;

// Boid on core 0
fix15 boid0_x ;
fix15 boid0_y ;
fix15 boid0_vx ;
fix15 boid0_vy ;

// Boid on core 1
fix15 boid1_x ;
fix15 boid1_y ;
fix15 boid1_vx ;
fix15 boid1_vy ;

// Create a boid
void spawnBoid(fix15* x, fix15* y, fix15* vx, fix15* vy, int direction)
{
  // increment numberOfBoids
  numberOfBoids++;
  // Start in center of screen
  *x = int2fix15(320) ;
  *y = int2fix15(240) ;
  // Choose left or right
  if (direction) *vx = int2fix15(3) ;
  else *vx = int2fix15(-3) ;
  // Moving down
  *vy = int2fix15(1) ;
}

// Draw the boundaries
void drawArena() {
  drawVLine(100, 100, 280, WHITE) ;
  drawVLine(540, 100, 280, WHITE) ;
  drawHLine(100, 100, 440, WHITE) ;
  drawHLine(100, 380, 440, WHITE) ;
}

// Detect wallstrikes, update velocity and position
void wallsAndEdges(fix15* x, fix15* y, fix15* vx, fix15* vy)
{
  // Zero all accumulator variables (can't do this in one line in C)
  // float xpos_avg = 0;
  // float ypos_avg= 0;
  // float xvel_avg = 0;
  // float yvel_avg = 0;
  // float neighboring_boids = 0;
  // float close_dx = 0;
  // float close_dy = 0;

  if (hitTop(*y)) {
    *vy = *vy + turnfactor;
  } 
  if (hitRight(*x)) {
    *vx = *vx - turnfactor;
  }
  if (hitLeft(*x)) {
    *vx = *vx + turnfactor;
  } 
  if (hitBottom(*y)) {
    *vy = *vy - turnfactor;
  } 

  float float_vx = fix2float15(*vx);
  float float_vy = fix2float15(*vy);
  float speed = sqrt(float_vx * float_vx + float_vy * float_vy);

  if (float2fix15(speed) < minspeed) {
        *vx = multfix15(divfix(*vx, float2fix15(speed)), minspeed);
        *vy = multfix15(divfix(*vy, float2fix15(speed)), minspeed);
  } else if (float2fix15(speed) > maxspeed) {
        *vx = multfix15(divfix(*vx, float2fix15(speed)), maxspeed);
        *vy = multfix15(divfix(*vy, float2fix15(speed)), maxspeed);
  }

  // Update position using velocity
  *x = *x + *vx ;
  *y = *y + *vy ;
}

// ==================================================
// === users serial input thread
// ==================================================
static PT_THREAD (protothread_serial(struct pt *pt))
{
    PT_BEGIN(pt);
    // stores user input
    static int user_input ;
    // wait for 0.1 sec
    PT_YIELD_usec(1000000) ;
    // announce the threader version
    sprintf(pt_serial_out_buffer, "Protothreads RP2040 v1.0\n\r");
    // non-blocking write
    serial_write ;
      while(1) {
        // print prompt
        sprintf(pt_serial_out_buffer, "input a number in the range 1-7: ");
        // non-blocking write
        serial_write ;
        // spawn a thread to do the non-blocking serial read
        serial_read ;
        // convert input string to number
        sscanf(pt_serial_in_buffer,"%d", &user_input) ;
        // update boid color
        if ((user_input > 0) && (user_input < 8)) {
          color = (char)user_input ;
        }
      } // END WHILE(1)
  PT_END(pt);
} // timer thread

// Animation on core 0
static PT_THREAD (protothread_anim(struct pt *pt))
{
    // Mark beginning of thread
    PT_BEGIN(pt);

    // Variables for maintaining frame rate
    static int begin_time ;
    static int spare_time ;

    // Spawn a boid
    for (int i = 0; i < 100; ++i) {
      spawnBoid(&boid0_x, &boid0_y, &boid0_vx, &boid0_vy, 0);
    }

    char src[50], lmao[50], lol[50];
    char numberOfBoids_str[50];
    char elapsedTime_str[50];
    char spareTime_str[50];
    strcpy(numberOfBoids_str, "Number of boids: ");

    setTextColor(RED) ;
    setCursor(0, 0) ;
    setTextSize(1) ;
    sprintf(src, "%d", numberOfBoids);
    strcat(numberOfBoids_str, src);
    writeString(numberOfBoids_str) ;

    setCursor(0, 10) ;
    writeString("Elapsed time: ");
    setCursor(0, 20) ;
    writeString("Spare time: ");

    while(1) {
      // Measure time at start of thread
      begin_time = time_us_32() ;      
      //printf("Time elapsed: %d\n", begin_time );
      // erase boid
      drawRect(fix2int15(boid0_x), fix2int15(boid0_y), 2, 2, BLACK);

      fillRect(100, 10, 50, 70, BLACK);
      setTextColor(RED) ;
      setCursor(100, 10) ;
      setTextSize(1) ;
      sprintf(lmao, "%d", begin_time / 1000000);
      strcat(elapsedTime_str, lmao);
      writeString(elapsedTime_str) ;
      memset(elapsedTime_str, '\0', sizeof(lmao));



      // update boid's position and velocity
      wallsAndEdges(&boid0_x, &boid0_y, &boid0_vx, &boid0_vy) ;
      // draw the boid at its new position
      drawRect(fix2int15(boid0_x), fix2int15(boid0_y), 2, 2, color); 
      // draw the boundaries
      drawArena() ;
      // delay in accordance with frame rate
      spare_time = FRAME_RATE - (time_us_32() - begin_time) ;

      sprintf(lol, "%d", spare_time);
      setTextColor(RED) ;
      setCursor(100, 20) ;
      setTextSize(1) ;
      strcat(spareTime_str, lol);
      writeString(spareTime_str) ;
      memset(spareTime_str, '\0', sizeof(lmao));


      // yield for necessary amount of time
      PT_YIELD_usec(spare_time) ;
     // NEVER exit while
    } // END WHILE(1)
  PT_END(pt);
} // animation thread


// Animation on core 1
static PT_THREAD (protothread_anim1(struct pt *pt))
{
    // Mark beginning of thread
    PT_BEGIN(pt);

    // Variables for maintaining frame rate
    static int begin_time ;
    static int spare_time ;

    // Spawn a boid
    spawnBoid(&boid1_x, &boid1_y, &boid1_vx, &boid1_vy, 1);

    while(1) {
      // Measure time at start of thread
      begin_time = time_us_32() ;      
      // erase boid
      drawRect(fix2int15(boid1_x), fix2int15(boid1_y), 2, 2, BLACK);
      // update boid's position and velocity
      wallsAndEdges(&boid1_x, &boid1_y, &boid1_vx, &boid1_vy) ;
      // draw the boid at its new position
      drawRect(fix2int15(boid1_x), fix2int15(boid1_y), 2, 2, color); 
      // delay in accordance with frame rate
      spare_time = FRAME_RATE - (time_us_32() - begin_time) ;
      // yield for necessary amount of time
      PT_YIELD_usec(spare_time) ;
     // NEVER exit while
    } // END WHILE(1)
  PT_END(pt);
} // animation thread

// ========================================
// === core 1 main -- started in main below
// ========================================
void core1_main(){
  // Add animation thread
  pt_add_thread(protothread_anim1);
  // Start the scheduler
  pt_schedule_start ;

}

// ========================================
// === main
// ========================================
// USE ONLY C-sdk library
int main(){
  // initialize stio
  stdio_init_all() ;

  // initialize VGA
  initVGA() ;

  // start core 1 
  // comment out core 1
  // multicore_reset_core1();
  // multicore_launch_core1(&core1_main);

  // add threads
  boid0_x = rand() % (540 + 1 - 100) + 100;
  boid0_y = rand() % (380 + 1 - 100) + 100;
  pt_add_thread(protothread_serial);
  pt_add_thread(protothread_anim);

  // start scheduler
  pt_schedule_start ;
} 
