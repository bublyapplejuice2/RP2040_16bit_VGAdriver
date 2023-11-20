#include "vga_graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

typedef unsigned short ushort;
#define MAX_GPIO_PINS 29

// #include "pt_cornell_rp2040_v1.h"



// R = 0-4
// G = 5-10
// B = 11-15

volatile int curr_mem_addr = 0;
volatile int count = 0;



void write_mem_addr_to_gpio() {
    gpio_put(4, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(5, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(6, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(7, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(8, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(20, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(19, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(18, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(17, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(16, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(15, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(14, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(13, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(12, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(11, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(2, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(1, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(0, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
    gpio_put(10, curr_mem_addr & 1);
    curr_mem_addr >>= 1;
}

void init_addr_write() {
    gpio_set_dir(10, GPIO_OUT);
    gpio_set_dir(0, GPIO_OUT);
    gpio_set_dir(1, GPIO_OUT);
    gpio_set_dir(2, GPIO_OUT);
    gpio_set_dir(11, GPIO_OUT);
    gpio_set_dir(12, GPIO_OUT);
    gpio_set_dir(13, GPIO_OUT);
    gpio_set_dir(14, GPIO_OUT);
    gpio_set_dir(15, GPIO_OUT);
    gpio_set_dir(16, GPIO_OUT);
    gpio_set_dir(17, GPIO_OUT);
    gpio_set_dir(18, GPIO_OUT);
    gpio_set_dir(19, GPIO_OUT);
    gpio_set_dir(20, GPIO_OUT);
    gpio_set_dir(8, GPIO_OUT);
    gpio_set_dir(7, GPIO_OUT);
    gpio_set_dir(6, GPIO_OUT);
    gpio_set_dir(5, GPIO_OUT);
    gpio_set_dir(4, GPIO_OUT);
}

void write() {
    gpio_put(22, 0);
    gpio_put(21, 0); // pull down write enable, begin writes
    write_mem_addr_to_gpio();
    curr_mem_addr++;
    gpio_put(22, 1);
}

void write(uint gpio, uint32_t events) {
    write();
}

void init_addr_read() {
    gpio_set_dir(10, GPIO_IN);
    gpio_set_dir(0, GPIO_IN);
    gpio_set_dir(1, GPIO_IN);
    gpio_set_dir(2, GPIO_IN);
    gpio_set_dir(11, GPIO_IN);
    gpio_set_dir(12, GPIO_IN);
    gpio_set_dir(13, GPIO_IN);
    gpio_set_dir(14, GPIO_IN);
    gpio_set_dir(15, GPIO_IN);
    gpio_set_dir(16, GPIO_IN);
    gpio_set_dir(17, GPIO_IN);
    gpio_set_dir(18, GPIO_IN);
    gpio_set_dir(19, GPIO_IN);
    gpio_set_dir(20, GPIO_IN);
    gpio_set_dir(8, GPIO_IN);
    gpio_set_dir(7, GPIO_IN);
    gpio_set_dir(6, GPIO_IN);
    gpio_set_dir(5, GPIO_IN);
    gpio_set_dir(4, GPIO_IN);
}

void read(uint gpio, uint32_t events) {
    curr_mem_addr = 0;
    int temp_mem_addr = 0;
    gpio_put(21, 1);
    gpio_put(3, 0);
    for (; curr_mem_addr < 480000; curr_mem_addr++) {
        temp_mem_addr = curr_mem_addr;
        if (gpio_get(27) == 0) {
            curr_mem_addr = 0;
            write_mem_addr_to_gpio();
        }
        while (gpio_get(27) == 0) {
            ;
        }
        curr_mem_addr = temp_mem_addr;
        write_mem_addr_to_gpio();
    }
}

int main(void) {
    // init_addr_write();
    for (int i = 0; i < MAX_GPIO_PINS; ++i) {
        gpio_init(i);
    }
    init_addr_write();
    
    // initVGA();

    // initialize GPIO pin as output
    gpio_set_dir(26, GPIO_IN);
    gpio_set_dir(22, GPIO_OUT);
    gpio_set_dir(21, GPIO_OUT);
    init_addr_write();

    // gpio_set_irq_enabled_with_callback(26, GPIO_IRQ_EDGE_RISE, true, &read);
    // gpio_set_irq_enabled_with_callback(22, GPIO_IRQ_LEVEL_HIGH, true, &write);
    
    while (true) {
        // gpio_put(22, 0);
        // sleep_us(1);
        // gpio_put(22, 1);
        // sleep_us(1);
        write();
    }
    
    return 0;
}