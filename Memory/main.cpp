#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/sync.h"
#include "vga_graphics.h"

#include "const_arr.h"

int i = 0;

char * token;
signed short x = -1;
signed short y = 0;

char buffer[3072];

static void core1_drawLine() {
    while (1) {
        scanf("%3071s", buffer) ;
        token = strtok(buffer, ",");
        while (token != NULL) {
            gpio_put(25, 1);
            if (x == -1) {
                y = atoi(token);
            }
            else {
                int pixel = ((400 * y ) + x) ;
                drawPixel(pixel, atoi(token));
            }
            x++;
            token = strtok(NULL, ",");
        }
        gpio_put(25, 0);
        x = -1;
        printf("done\n");
    }
}

void irqDisplay(uint gpio, uint32_t events) {
    if (gpio_get(26) == 0) {
        busy_wait_us_32(50000);
        if (gpio_get(26) == 0) {
            displayImage(images[i]);
            i = (i + 1) % 6;
        }
    }
}

int main(void) {
    set_sys_clock_khz(270000, true);
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    gpio_init(26);
    gpio_set_dir(26, GPIO_IN);
    gpio_pull_up(26);

    uart_inst_t *uart = uart0;
    uart_init(uart, 115200);
    gpio_set_function(16, GPIO_FUNC_UART);
    gpio_set_function(17, GPIO_FUNC_UART);

    initVGA();

    multicore_reset_core1();
    multicore_launch_core1(core1_drawLine);
    
    gpio_set_irq_enabled_with_callback(26, GPIO_IRQ_EDGE_FALL, true, &irqDisplay);

    while (1) {
        ;
    }

    return 0;

        
}