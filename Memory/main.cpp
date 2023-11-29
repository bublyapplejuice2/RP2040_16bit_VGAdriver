#include "vga_graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

// #include "pt_cornell_rp2040_v1.h"

int main(void) {
    //set_sys_clock_khz(250000, true);
    stdio_init_all();

    uart_inst_t *uart = uart0;
    uart_init(uart, 115200);
    gpio_set_function(16, GPIO_FUNC_UART);
    gpio_set_function(17, GPIO_FUNC_UART);


    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);

    initVGA();
    
    unsigned short color = 0;
    while (true) {
        fillRect(0, 0, 319, 240, GREEN);
    }

}