#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

typedef unsigned short ushort;

// #include "pt_cornell_rp2040_v1.h"



// R = 0-4
// G = 5-10
// B = 11-15

volatile int curr_mem_addr=0;

void write(uint gpio, uint32_t events) {
    gpio_put(21, 0);
    gpio_put(21, 1);
    gpio_set_dir(22, GPIO_OUT);
    gpio_pull_down(22);
    gpio_set_dir(22, GPIO_IN);
    gpio_set_dir(26, GPIO_OUT);
    gpio_pull_down(26);
    gpio_set_dir(26, GPIO_IN);
}

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

void next_address(uint gpio, uint32_t events) {
    // current address increment by 1
    curr_mem_addr++;
    write_mem_addr_to_gpio();
}

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

void init_addr_write() {
    gpio_init(10);
    gpio_set_dir(10, GPIO_OUT);
    gpio_init(0);
    gpio_set_dir(0, GPIO_OUT);
    gpio_init(1);
    gpio_set_dir(1, GPIO_OUT);
    gpio_init(2);
    gpio_set_dir(2, GPIO_OUT);
    gpio_init(11);
    gpio_set_dir(11, GPIO_OUT);
    gpio_init(12);
    gpio_set_dir(12, GPIO_OUT);
    gpio_init(13);
    gpio_set_dir(13, GPIO_OUT);
    gpio_init(14);
    gpio_set_dir(14, GPIO_OUT);
    gpio_init(15);
    gpio_set_dir(15, GPIO_OUT);
    gpio_init(16);
    gpio_set_dir(16, GPIO_OUT);
    gpio_init(17);
    gpio_set_dir(17, GPIO_OUT);
    gpio_init(18);
    gpio_set_dir(18, GPIO_OUT);
    gpio_init(19);
    gpio_set_dir(19, GPIO_OUT);
    gpio_init(20);
    gpio_set_dir(20, GPIO_OUT);
    gpio_init(8);
    gpio_set_dir(8, GPIO_OUT);
    gpio_init(7);
    gpio_set_dir(7, GPIO_OUT);
    gpio_init(6);
    gpio_set_dir(6, GPIO_OUT);
    gpio_init(5);
    gpio_set_dir(5, GPIO_OUT);
    gpio_init(4);
    gpio_set_dir(4, GPIO_OUT);
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
    init_addr_read();
    for (; curr_mem_addr < 480000; curr_mem_addr++) {
        write_mem_addr_to_gpio();
    }
}

int main(void) {
    set_sys_clock_khz(258000, true);
    gpio_init(22);
    gpio_set_dir(22, GPIO_IN);
    gpio_init(26);
    gpio_set_dir(26, GPIO_IN);
    gpio_set_irq_enabled_with_callback(22, GPIO_IRQ_EDGE_RISE, true, &write);
    gpio_set_irq_enabled_with_callback(26, GPIO_IRQ_EDGE_RISE, true, &next_address);
    gpio_init(27);
    gpio_set_dir(27, GPIO_IN);
    gpio_set_irq_enabled_with_callback(27, GPIO_IRQ_EDGE_RISE, true, &read);
    while (1) {
        sleep_ms(5000);
    }
    return 0;
}