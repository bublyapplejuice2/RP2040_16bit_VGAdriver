#include "vga_graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

// #include "pt_cornell_rp2040_v1.h"

int main(void) {
    set_sys_clock_khz(264000, true);
    stdio_init_all();
    initVGA();
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);
    unsigned short color = 0;
    while (true) {
        fillRect(0, 0, 300, 200, 42000);
    }
    
}