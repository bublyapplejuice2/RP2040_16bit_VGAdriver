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
typedef signed int fix15;
#define multfix15(a, b)((fix15)((((signed long long)(a)) * ((signed long long)(b))) >> 15))
#define float2fix15(a)((fix15)((a) * 32768.0)) // 2^15
#define fix2float15(a)((float)(a) / 32768.0)
#define absfix15(a) abs(a)
#define int2fix15(a)((fix15)(a << 15))
#define fix2int15(a)((int)(a >> 15))
#define char2fix15(a)(fix15)(((fix15)(a)) << 15)
#define divfix(a, b)(fix15)(div_s64s64((((signed long long)(a)) << 15), ((signed long long)(b))))

int bottom = 380;
int top = 100;
int left = 100;
int right = 520;

int no_bounding = 0;

// Wall detection
#define hitBottom(b)(b > int2fix15(bottom))
#define hitTop(b)(b < int2fix15(top))
#define hitLeft(a)(a < int2fix15(left))
#define hitRight(a)(a > int2fix15(right))

volatile fix15 turnfactor = float2fix15(0.2);
volatile fix15 visualRange = int2fix15(40);
volatile fix15 protectedRange = int2fix15(8);
volatile fix15 centeringfactor = float2fix15(0.0005);
volatile fix15 avoidfactor = float2fix15(0.05);
volatile fix15 matchingfactor = float2fix15(0.05);
volatile fix15 maxspeed = int2fix15(6);
volatile fix15 minspeed = int2fix15(3);
volatile fix15 predatorturnfactor = float2fix15(0.5);
volatile fix15 predatorRange = int2fix15(100);

// uS per frame
#define FRAME_RATE 33000

// the color of the boid
char color = WHITE;

// total number of boids
volatile int numberOfBoids = 0;
#define spawnedBoids 750
#define spawnedBoidsHalved spawnedBoids / 2

fix15 boid_x[spawnedBoids];
fix15 boid_y[spawnedBoids];
fix15 boid_vx[spawnedBoids];
fix15 boid_vy[spawnedBoids];

// Fast approximation to the sqrt
float Q_sqrt(float z) {
	union { float f; uint32_t i; } val = {z};
	val.i -= 1 << 23;
	val.i >>= 1;
	val.i += 1 << 29;
	return val.f;
}

// Create a boid
void spawnBoid(fix15 * x, fix15 * y, fix15 * vx, fix15 * vy, int direction, int idx) {
  // increment numberOfBoids
  numberOfBoids++;
  // Start in center of screen
  // 640 x 480
  int x_dir = rand() % (620 + 1 - 50) + 50;
  int y_dir = rand() % (440 + 1 - 50) + 50;
  * x = int2fix15(x_dir);
  * y = int2fix15(y_dir);
  int x_vel = rand() % (6 + 1 - 3) + 3;
  int y_vel = Q_sqrt(36 - x_vel);
  // Choose left or right
  if (direction) {
    * vx = int2fix15(x_vel);
    * vy = int2fix15(y_vel);
  }
  else {
    * vx = int2fix15(-x_vel);
    * vy = int2fix15(-y_vel);
  }
  // Moving down
}

volatile int checkNeighbors = 0;

// Draw the boundaries
void drawArena() {
  if (no_bounding) {
    drawVLine(left, top, bottom - top, BLACK);
    drawVLine(right, top, bottom - top, BLACK);
    drawHLine(left, top, right - left, BLACK);
    drawHLine(left, bottom, right - left, BLACK);
  } else {
    drawVLine(left, top, bottom - top, WHITE);
    drawVLine(right, top, bottom - top, WHITE);
    drawHLine(left, top, right - left, WHITE);
    drawHLine(left, bottom, right - left, WHITE);
  }
}

// Detect wallstrikes, update velocity and position
void wallsAndEdges(fix15 * x, fix15 * y, fix15 * vx, fix15 * vy, int idx) {
  // Zero all accumulator variables (can't do this in one line in C)
  fix15 xpos_avg = int2fix15(0);
  fix15 ypos_avg = int2fix15(0);
  fix15 xvel_avg = int2fix15(0);
  fix15 yvel_avg = int2fix15(0);
  fix15 neighboring_boids = int2fix15(0);
  fix15 close_dx = int2fix15(0);
  fix15 close_dy = int2fix15(0);

  for (int i = 0; i < spawnedBoids; i=i+10) {
    if (i != idx) {
      fix15 dx = * x - boid_x[i];
      fix15 dy = * y - boid_y[i];

      if (absfix15(dx) < visualRange && absfix15(dy) < visualRange) {
        fix15 squared_distance = multfix15(dx, dx) + multfix15(dy, dy);

        if (squared_distance < multfix15(protectedRange, protectedRange)) {
          close_dx = close_dx + (*x - boid_x[i]);
          close_dy = close_dy + (*y - boid_y[i]);
        } else if (squared_distance < multfix15(visualRange, visualRange)) {
          xpos_avg = xpos_avg + boid_x[i]; 
          ypos_avg = ypos_avg + boid_y[i]; 
          xvel_avg = xvel_avg + boid_vx[i];
          yvel_avg = yvel_avg + boid_vy[i];

          neighboring_boids = neighboring_boids + int2fix15(1);
        }
      }
    }
  }

  if (neighboring_boids > int2fix15(0)) {
    xpos_avg = divfix(xpos_avg, neighboring_boids);
    ypos_avg = divfix(ypos_avg, neighboring_boids);
    xvel_avg = divfix(xvel_avg, neighboring_boids);
    yvel_avg = divfix(yvel_avg, neighboring_boids);
    //xpos_avg = xpos_avg >> 2;
    //ypos_avg = ypos_avg >> 2;
    //xvel_avg = xvel_avg >> 3;
    //yvel_avg = yvel_avg >> 3;


    *vx = *vx + multfix15(xpos_avg - *x,centeringfactor) + 
                multfix15(xvel_avg - *vx, matchingfactor);
    *vy = *vy + multfix15(ypos_avg - *y,centeringfactor) + 
                multfix15(yvel_avg - *vy, matchingfactor);
  }

  *vx = *vx + multfix15(close_dx, avoidfactor);
  *vy = *vy + multfix15(close_dy, avoidfactor);

  int x_pos = fix2int15(*x);
  int y_pos = fix2int15(*y);

  if (no_bounding == 1) {
    if (x_pos < 5 || x_pos > 620) {
      if (x_pos < 5) {
        x_pos = (620 - x_pos);
      } else if (x_pos > 620) {
        x_pos = x_pos - 620;
      }
    }
    if (y_pos < 5 || y_pos > 440) {
      if (y_pos < 5) {
        y_pos = (440 - y_pos);
      } else if (y_pos > 440) {
        y_pos = y_pos - 440;
      }
    }

    *x = int2fix15(x_pos);
    *y = int2fix15(y_pos);
  } else {
    if (hitTop( * y)) {
      * vy = * vy + turnfactor;
    }
    if (hitRight( * x)) {
      * vx = * vx - turnfactor;
    }
    if (hitLeft( * x)) {
      * vx = * vx + turnfactor;
    }
    if (hitBottom( * y)) {
      * vy = * vy - turnfactor;
    }
  }

  float float_vx = fix2float15( * vx);
  float float_vy = fix2float15( * vy);
  float speed = Q_sqrt(float_vx * float_vx + float_vy * float_vy);

  if (float2fix15(speed) < minspeed) {
    * vx = multfix15(divfix( * vx, float2fix15(speed)), minspeed);
    * vy = multfix15(divfix( * vy, float2fix15(speed)), minspeed);
  } else if (float2fix15(speed) > maxspeed) {
    * vx = multfix15(divfix( * vx, float2fix15(speed)), maxspeed);
    * vy = multfix15(divfix( * vy, float2fix15(speed)), maxspeed);
  }

  // Update position using velocity
  * x = * x + * vx;
  * y = * y + * vy;
}

// ==================================================
// === users serial input thread
// ==================================================
static PT_THREAD(protothread_serial(struct pt * pt)) {
  PT_BEGIN(pt);
  // stores user input
  static int user_input;
  float user_centering_factor;
  int user_box_direction;
  int user_box_factor;
  int user_visual_range;
  int user_protected_range;
  float user_avoid_factor;
  float user_matching_factor;
  // wait for 0.1 sec
  PT_YIELD_usec(1000000);
  // non-blocking write
  serial_write;

  while (1) {
    // print prompt
    sprintf(pt_serial_out_buffer, "input a number in the range 1-6: ");
    serial_write;
    serial_read;
    sscanf(pt_serial_in_buffer, "%d", & user_input);
    // update 
    if (user_input == 1) {
      // changes the centering factor
      sprintf(pt_serial_out_buffer, "the centering factor can be any float between 0 and 1: ");
      serial_write;
      serial_read;
      sscanf(pt_serial_in_buffer, "%f", & user_centering_factor);
      if (user_centering_factor > 1.0 || user_centering_factor < 0) {
        sprintf(pt_serial_out_buffer, "invalid float entered\r\n");
        serial_write;
      } else {
        centeringfactor = float2fix15(user_centering_factor);
        sprintf(pt_serial_out_buffer, "centering factor set to %f\r\n", fix2float15(centeringfactor));
        serial_write;
      }
      user_input = -1;
    } else if (user_input == 2) {
        sprintf(pt_serial_out_buffer, "bounds disabled\r\n");
        serial_write;
        no_bounding = no_bounding ^ 1;
    } else if (user_input == 3) {
      //changes the visual range
      sprintf(pt_serial_out_buffer, "the visual range can be any int: ");
      serial_write;
      serial_read;
      sscanf(pt_serial_in_buffer, "%d", & user_visual_range);
      visualRange = int2fix15(user_visual_range);
      sprintf(pt_serial_out_buffer, "visual range set to %d\r\n", fix2int15(visualRange));
      serial_write;
      user_input = -1;
    } else if (user_input == 4) {
      //changes the protected range
      sprintf(pt_serial_out_buffer, "the protected range can be any int: ");
      serial_write;
      serial_read;
      sscanf(pt_serial_in_buffer, "%d", & user_protected_range);
      protectedRange = int2fix15(user_protected_range);
      sprintf(pt_serial_out_buffer, "protected range set to %d\r\n", fix2int15(protectedRange));
      serial_write;
      user_input = -1;
    } else if (user_input == 5) {
      //changes the avoid factor
      sprintf(pt_serial_out_buffer, "the avoid factor can be any float between 0 and 1: ");
      serial_write;
      serial_read;
      sscanf(pt_serial_in_buffer, "%f", & user_avoid_factor);
      if (user_avoid_factor > 1.0 || user_avoid_factor < 0) {
        sprintf(pt_serial_out_buffer, "invalid float entered\r\n");
        serial_write;
      } else {
        avoidfactor = float2fix15(user_avoid_factor);
        sprintf(pt_serial_out_buffer, "avoid factor set to %f\r\n", fix2float15(avoidfactor));
        serial_write;
      }
      user_input = -1;
    } else if (user_input == 6) {
      //changes the matching factor
      sprintf(pt_serial_out_buffer, "the matching factor can be any float between 0 and 1: ");
      serial_write;
      serial_read;
      sscanf(pt_serial_in_buffer, "%f", & user_matching_factor);
      if (user_matching_factor > 1.0 || user_matching_factor < 0) {
        sprintf(pt_serial_out_buffer, "invalid float entered\r\n");
        serial_write;
      } else {
        matchingfactor = float2fix15(user_matching_factor);
        sprintf(pt_serial_out_buffer, "matching factor set to %f\r\n", fix2float15(matchingfactor));
        serial_write;
      }
      user_input = -1;
    }
  } // END WHILE(1)
  PT_END(pt);
} // timer thread

// Animation on core 0
static PT_THREAD(protothread_anim(struct pt * pt)) {
  // Mark beginning of thread
  PT_BEGIN(pt);

  // Variables for maintaining frame rate
  static int begin_time;
  static int spare_time;

  char numberOfBoids_str[85];
  char elapsedTime_str[75];
  char spareTime_str[95];

  int skip = 0;

  while (1) {
    skip++;
    if (skip == 10) skip = 0;
    // Measure time at start of thread
    begin_time = time_us_32();

    setCursor(0, 0);
    sprintf(numberOfBoids_str, "%d%s", numberOfBoids, " boids");
    writeString(numberOfBoids_str);

    if (skip == 0) {
      fillRect(0, 0, 105, 30, BLACK);
      setTextColor(RED);
      setTextSize(1);

      setCursor(0, 10);
      sprintf(elapsedTime_str, "%s%d", "Elapsed time: ", begin_time >> 20);
      writeString(elapsedTime_str);
    }

    // erase boid, update boid's position and velocity, draw boid
    for (int i = 0; i < spawnedBoidsHalved; ++i) {
      drawRect(fix2int15(boid_x[i]), fix2int15(boid_y[i]), 2, 2, BLACK);
      wallsAndEdges( & boid_x[i], & boid_y[i], & boid_vx[i], & boid_vy[i], i);
      drawRect(fix2int15(boid_x[i]), fix2int15(boid_y[i]), 2, 2, color);
    }

    // draw the boundaries
    drawArena();
    // delay in accordance with frame rate
    spare_time = FRAME_RATE - (time_us_32() - begin_time);

    if (skip == 0) {
      setCursor(0, 20);
      sprintf(spareTime_str, "%s%d", "Spare time: ", spare_time);
      writeString(spareTime_str);
    }

    // yield for necessary amount of time
    PT_YIELD_usec(spare_time);
    // NEVER exit while
  } // END WHILE(1)
  PT_END(pt);
} // animation thread

// Animation on core 1
static PT_THREAD(protothread_anim1(struct pt * pt)) {
  // Mark beginning of thread
  PT_BEGIN(pt);

  // Variables for maintaining frame rate
  static int begin_time;
  static int spare_time;

  while (1) {
    // Measure time at start of thread
    begin_time = time_us_32();

    // erase boid, update boid's position and velocity, draw boid
    for (int i = spawnedBoidsHalved ; i < spawnedBoids; ++i) {
      drawRect(fix2int15(boid_x[i]), fix2int15(boid_y[i]), 2, 2, BLACK);
      wallsAndEdges( & boid_x[i], & boid_y[i], & boid_vx[i], & boid_vy[i], i);
      drawRect(fix2int15(boid_x[i]), fix2int15(boid_y[i]), 2, 2, color);
    }

    // delay in accordance with frame rate
    spare_time = FRAME_RATE - (time_us_32() - begin_time);
    // yield for necessary amount of time
    PT_YIELD_usec(spare_time);
    // NEVER exit while
  } // END WHILE(1)
  PT_END(pt);
} // animation thread

// ========================================
// === core 1 main -- started in main below
// ========================================
void core1_main() {
  // Add animation thread
  pt_add_thread(protothread_anim1);
  // Start the scheduler
  pt_schedule_start;

}

// ========================================
// === main
// ========================================
// USE ONLY C-sdk library
int main() {
  // initialize stio
  stdio_init_all();

  // initialize VGA
  initVGA();

  for (int i = 0; i < spawnedBoids; ++i) {
    spawnBoid( & boid_x[i], & boid_y[i], & boid_vx[i], & boid_vy[i], i % 2, i);
  }

  multicore_reset_core1();
  multicore_launch_core1(&core1_main);

  pt_add_thread(protothread_serial);
  pt_add_thread(protothread_anim);

  // start scheduler
  pt_schedule_start;
}