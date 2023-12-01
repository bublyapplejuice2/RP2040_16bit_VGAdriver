#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include <cstring>
#include "vga_graphics.h"
// #include "const_arr.h"

char * token;
unsigned int pixel = 0;

int main(void) {
    set_sys_clock_khz(270000, true);
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    uart_inst_t *uart = uart0;
    uart_init(uart, 115200);
    gpio_set_function(16, GPIO_FUNC_UART);
    gpio_set_function(17, GPIO_FUNC_UART);

    initVGA();

    while (1) {
        // if buffer is not empty, read from it
        char buffer[4096];
        scanf("%4096s", buffer) ;
        
        token = strtok(buffer, " ");
        while (token != NULL) {
            gpio_put(25, 1);
            drawPixel(pixel, atoi(token));
            pixel++;
            token = strtok(NULL, " ");
        }
        gpio_put(25, 0);
    }
}