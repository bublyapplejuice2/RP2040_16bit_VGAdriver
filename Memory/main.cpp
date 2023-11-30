#include "vga_graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

// #include "pt_cornell_rp2040_v1.h"

int main(void) {
    set_sys_clock_khz(270000, true);
    stdio_init_all();

    uart_inst_t *uart = uart0;
    uart_init(uart, 115200);
    gpio_set_function(16, GPIO_FUNC_UART);
    gpio_set_function(17, GPIO_FUNC_UART);


    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);

    initVGA();
    
    unsigned short color = 10;
    fillRect(0, 0, 399, 299, GREEN);
    // for (int i = 0; i < 320; ++i) {
    //     for (int j = 0; j < 240; ++j) {
    //         drawPixel(i, j, color);
    //         color += 100;
    //     }
    // }
}