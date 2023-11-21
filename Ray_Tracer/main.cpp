//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

// #include "rtweekend.h"

// #include "camera.h"
// #include "color.h"
// #include "hittable_list.h"
// #include "material.h"
// #include "sphere.h"


#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

typedef unsigned short ushort;
volatile ushort color = 0;
volatile int count = 0;

#define MAX_GPIO_PINS 29

// #include "pt_cornell_rp2040_v1.h"

// R = 0-4
// G = 5-10
// B = 11-15

volatile int color_to_use;


void get_mem_addr(int (&gpio_arr)[19], int (&mem_arr)[19]) {
    mem_arr[18] = gpio_arr[8]; // A18 --> GPIO 10
    mem_arr[17] = gpio_arr[0];  // A17 --> GPIO 0
    mem_arr[16] = gpio_arr[1];  // A16 --> GPIO 1
    mem_arr[15] = gpio_arr[2];  // A15 --> GPIO 2
    mem_arr[14] = gpio_arr[9]; // A14 --> GPIO 11
    mem_arr[13] = gpio_arr[10]; // A13 --> GPIO 12
    mem_arr[12] = gpio_arr[11]; // A12 --> GPIO 13
    mem_arr[11] = gpio_arr[12]; // A11 --> GPIO 14
    mem_arr[10] = gpio_arr[13]; // A10 --> GPIO 15
    mem_arr[9]  = gpio_arr[14]; // A9 --> GPIO 16
    mem_arr[8]  = gpio_arr[15]; // A8 --> GPIO 17
    mem_arr[7]  = gpio_arr[16]; // A7 --> GPIO 18
    mem_arr[6]  = gpio_arr[17]; // A6 --> GPIO 19
    mem_arr[5]  = gpio_arr[18]; // A5 --> GPIO 20
    mem_arr[4]  = gpio_arr[7];  // A4 --> GPIO 8
    mem_arr[3]  = gpio_arr[6];  // A3 --> GPIO 7
    mem_arr[2]  = gpio_arr[5];  // A2 --> GPIO 6
    mem_arr[1]  = gpio_arr[4];  // A1 --> GPIO 5
    mem_arr[0]  = gpio_arr[3];  // A0 --> GPIO 4
}

void get_gpio_arr(int (&gpio_arr)[19], int (&mem_arr)[19]) {
    gpio_arr[8]   = mem_arr[18];// A18 --> GPIO 10
    gpio_arr[0]   = mem_arr[17]; // A17 --> GPIO 0
    gpio_arr[1]   = mem_arr[16]; // A16 --> GPIO 1
    gpio_arr[2]   = mem_arr[15]; // A15 --> GPIO 2
    gpio_arr[9]   = mem_arr[14];// A14 --> GPIO 11
    gpio_arr[10]  = mem_arr[13]; // A13 --> GPIO 12
    gpio_arr[11]  = mem_arr[12]; // A12 --> GPIO 13
    gpio_arr[12]  = mem_arr[11]; // A11 --> GPIO 14
    gpio_arr[13]  = mem_arr[10]; // A10 --> GPIO 15
    gpio_arr[14]  = mem_arr[9]; // A9 --> GPIO 16
    gpio_arr[15]  = mem_arr[8]; // A8 --> GPIO 17
    gpio_arr[16]  = mem_arr[7]; // A7 --> GPIO 18
    gpio_arr[17]  = mem_arr[6]; // A6 --> GPIO 19
    gpio_arr[18]  = mem_arr[5]; // A5 --> GPIO 20
    gpio_arr[7]   = mem_arr[4]; // A4 --> GPIO 8
    gpio_arr[6]   = mem_arr[3]; // A3 --> GPIO 7
    gpio_arr[5]   = mem_arr[2]; // A2 --> GPIO 6
    gpio_arr[4]   = mem_arr[1]; // A1 --> GPIO 5
    gpio_arr[3]   = mem_arr[0]; // A0 --> GPIO 4
}

void set_color(ushort color) {
    gpio_put(0, color & 1);
    color >>= 1;
    gpio_put(1, color & 1);
    color >>= 1;
    gpio_put(2, color & 1);
    color >>= 1;
    gpio_put(3, color & 1);
    color >>= 1;
    gpio_put(4, color & 1);
    color >>= 1;
    gpio_put(5, color & 1);
    color >>= 1;
    gpio_put(6, color & 1);
    color >>= 1;
    gpio_put(7, color & 1);
    color >>= 1;
    gpio_put(8, color & 1);
    color >>= 1;
    gpio_put(9, color & 1);
    color >>= 1;
    gpio_put(10, color & 1);
    color >>= 1;
    gpio_put(11, color & 1);
    color >>= 1;
    gpio_put(12, color & 1);
    color >>= 1;
    gpio_put(13, color & 1);
    color >>= 1;
    gpio_put(14, color & 1);
    color >>= 1;
    gpio_put(15, color & 1);
    color >>= 1;
}

void incr_color(uint gpio, uint32_t events) {
    color ++;
    ushort temp = color;
    count ++;
    gpio_put(25, !gpio_get(25)) ;
    
    gpio_put(0, temp & 1);
    temp >>= 1;
    gpio_put(1, temp & 1);
    temp >>= 1;
    gpio_put(2, temp & 1);
    temp >>= 1;
    gpio_put(3, temp & 1);
    temp >>= 1;
    gpio_put(4, temp & 1);
    temp >>= 1;
    gpio_put(5, temp & 1);
    temp >>= 1;
    gpio_put(6, temp & 1);
    temp >>= 1;
    gpio_put(7, temp & 1);
    temp >>= 1;
    gpio_put(8, temp & 1);
    temp >>= 1;
    gpio_put(9, temp & 1);
    temp >>= 1;
    gpio_put(10, temp & 1);
    temp >>= 1;
    gpio_put(11, temp & 1);
    temp >>= 1;
    gpio_put(12, temp & 1);
    temp >>= 1;
    gpio_put(13, temp & 1);
    temp >>= 1;
    gpio_put(14, temp & 1);
    temp >>= 1;
    gpio_put(15, temp & 1);
    temp >>= 1;
}

int main(void) {
    set_sys_clock_khz(264000, true);
    stdio_init_all();
    
    for (int i = 0; i < 16; ++i) {
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
    }
    set_color(0);

    // initialize GPIO pin as output
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    gpio_init(21);
    gpio_set_dir(21, GPIO_IN);
    gpio_set_irq_enabled_with_callback(21, GPIO_IRQ_EDGE_RISE, true, &incr_color);

    // total pixels = 600 x 800 = 480000
    while (count < 480000) {
        ;
    }
    gpio_init(22);
    gpio_set_dir(22, GPIO_OUT);
    gpio_put(22, 1);
    
    for (int i = 0; i < 16; i++) {
        gpio_set_dir(i, GPIO_IN);
    }
    return 0;
}