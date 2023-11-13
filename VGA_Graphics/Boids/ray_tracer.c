/**
 *
 * HARDWARE CONNECTIONS
 * - A18 --> GPIO 10
 * - A17 --> GPIO 0
 * - A16 --> GPIO 1
 * - A15 --> GPIO 2
 * - A14 --> GPIO 11
 * - A13 --> GPIO 12
 * - A12 --> GPIO 13
 * - A11 --> GPIO 14
 * - A10 --> GPIO 15
 * - A9  --> GPIO 16
 * - A8  --> GPIO 17
 * - A7  --> GPIO 18
 * - A6  --> GPIO 19
 * - A5  --> GPIO 20
 * - A4  --> GPIO 8
 * - A3  --> GPIO 7
 * - A2  --> GPIO 6
 * - A1  --> GPIO 5
 * - A0  --> GPIO 4
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

// === the Hardware Connection Macros ========================================
#define A18 10
#define A17 0
#define A16 1
#define A15 2
#define A14 11
#define A13 12
#define A12 13
#define A11 14
#define A10 15
#define A9 16
#define A8 17
#define A7 18
#define A6 19
#define A5 20
#define A5 8
#define A5 7
#define A5 6
#define A5 5
#define A5 4

int gpio_macros[19] = {10, 0, 1, 2, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 8, 7, 6, 5, 4};


static PT_THREAD (protothread_serial(struct pt *pt))
{
    PT_BEGIN(pt) ;
    static unsigned int test_in = 0 ;
    while(1) {
        sprintf(pt_serial_out_buffer, "Input a color: \r\n");
        serial_write ;
        serial_read ;
        sscanf(pt_serial_in_buffer,"%d", &test_in) ;
        if ( test_in <= 65535 ) {
            
            test_in = 0;
        }
    }
    PT_END(pt) ;
}

// ========================================
// === main
// ========================================
// USE ONLY C-sdk library
int main() {
    stdio_init_all();

    for (int i = 0; i < 19; i++) {
    	gpio_init(gpio_macros[i]) ;
        gpio_set_dir(gpio_macros[i], GPIO_OUT) ;
        gpio_put(gpio_macros[i], 0) ;
    }

	pt_add_thread(protothread_serial) ;
    pt_schedule_start ;
    
}