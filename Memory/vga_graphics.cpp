#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
// Our assembled programs:
// Each gets the name <pio_filename.pio.h>
#include "hsync.pio.h"
#include "vsync.pio.h"
#include "rgb.pio.h"
#include "rgb2.pio.h"
#include "vga_graphics.h"

// VGA timing constants
#define H_ACTIVE   655    // (active + frontporch - 1) - one cycle delay for mov
#define V_ACTIVE   479    // (active - 1)
#define RGB_ACTIVE 319    // (horizontal active)/2 - 1
// #define RGB_ACTIVE 639 // change to this if 1 pixel/byte

// Length of the pixel array, and number of DMA transfers
#define TXCOUNT 76800 // Total pixels 320x240 (since we have 2 pixels per byte)
///#define TXCOUNT 30000

// Pixel color array that is DMA's to the PIO machines and
// a pointer to the ADDRESS of this color array.
// Note that this array is automatically initialized to all 0's (black)
unsigned short vga_data_array[TXCOUNT];
// points to address storing pointer to screen array
unsigned short * address_pointer = &vga_data_array[0] ;

// Screen width/height
#define _width 640
#define _height 480

void initVGA() {
        // Choose which PIO instance to use (there are two instances, each with 4 state machines)
    PIO pio = pio0;

    // Our assembled program needs to be loaded into this PIO's instruction
    // memory. This SDK function will find a location (offset) in the
    // instruction memory where there is enough space for our program. We need
    // to remember these locations!
    //
    // We only have 32 instructions to spend! If the PIO programs contain more than
    // 32 instructions, then an error message will get thrown at these lines of code.
    //
    // The program name comes from the .program part of the pio file
    // and is of the form <program name_program>
    uint hsync_offset = pio_add_program(pio, &hsync_program);
    uint vsync_offset = pio_add_program(pio, &vsync_program);
    uint rgb_offset = pio_add_program(pio, &rgb_program);
    uint rgb2_offset = pio_add_program(pio, &rgb2_program);

    // Manually select a few state machines from pio instance pio0.
    uint hsync_sm = 0;
    uint vsync_sm = 1;
    uint rgb_sm = 2;
    uint rgb_sm3 = 3;

    // Call the initialization functions that are defined within each PIO file.
    // Why not create these programs here? By putting the initialization function in
    // the pio file, then all information about how to use/setup that state machine
    // is consolidated in one place. Here in the C, we then just import and use it.
    hsync_program_init(pio, hsync_sm, hsync_offset, HSYNC);
    vsync_program_init(pio, vsync_sm, vsync_offset, VSYNC);
    rgb_program_init(pio, rgb_sm, rgb_offset, 0); //8
    rgb_program_init(pio, rgb_sm3, rgb2_offset, 0); //8
    // Start the two pio machine IN SYNC
    // Note that the RGB state machine is running at full speed,
    // so synchronization doesn't matter for that one. But, we'll
    // start them all simultaneously anyway.
    pio_enable_sm_mask_in_sync(pio, ((1u << hsync_sm) | (1u << vsync_sm) | (1u << rgb_sm) | (1u << rgb_sm3)));

    // turn up i/o pin drive
    // void gpio_set_drive_strength (uint gpio, enum gpio_drive_strength drive)
    for(int i=0; i<=15; i++){
        gpio_set_drive_strength (i, GPIO_DRIVE_STRENGTH_12MA);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // ============================== PIO DMA Channels =================================================
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // DMA channels - 0 and 2 sends color data, 1 and 3 reconfigures and restarts 0 and 2
    // the logic here is that the vsynch state machine enforces line doubling between the two data machines
    int rgb_chan_0 = 0; // sends data for one aframe then chains to 1
    int rgb_chan_1 = 1; // set beginning address for chan 0 then chains to  chan 0
    int rgb_chan_2 = 2; // sends data for one aframe then chains to 3
    int rgb_chan_3 = 3; // resets channel 2
    // ===============================================
    // Channel Zero (sends color data to PIO VGA machine)
    dma_channel_config c0 = dma_channel_get_default_config(rgb_chan_0);  // default configs
    channel_config_set_transfer_data_size(&c0, DMA_SIZE_16);              // 8-bit txfers
    channel_config_set_read_increment(&c0, true);                        // yes read incrementing
    channel_config_set_write_increment(&c0, false);                      // no write incrementing
    channel_config_set_dreq(&c0, DREQ_PIO0_TX2) ;                        // DREQ_PIO0_TX2 pacing (FIFO)
    channel_config_set_chain_to(&c0, rgb_chan_1);                        // chain to other channel

    dma_channel_configure(
        rgb_chan_0,                 // Channel to be configured
        &c0,                        // The configuration we just created
        &pio->txf[rgb_sm],          // write address (RGB PIO TX FIFO)
        &vga_data_array,            // place holder for first load from channel one
        TXCOUNT,                    // Number of transfers; in this case each is 1 byte.
        true                       // Don't start immediately.
    );

    // ===============================================
    // Channel One (reconfigures the channel 0 for next line)
    dma_channel_config c1 = dma_channel_get_default_config(rgb_chan_1);   // default configs
    channel_config_set_transfer_data_size(&c1, DMA_SIZE_32);              // 32-bit txfers
    channel_config_set_read_increment(&c1, false);                        // no read incrementing
    channel_config_set_write_increment(&c1, false);                       // no write incrementing
    channel_config_set_chain_to(&c1, rgb_chan_0);                         // chain to other channel

    dma_channel_configure(
        rgb_chan_1,                         // Channel to be configured
        &c1,                                // The configuration we just created
        &dma_hw->ch[rgb_chan_0].read_addr,  // Write address (channel 0 read address)
        &address_pointer,                    // Read address (POINTER TO AN ADDRESS)
        1,                                  // Number of transfers, in this case each is 4 byte
        false                               // Don't start immediately.
    );

     // ===============================================
    // Channel 2 (sends color data to PIO VGA machine)
    dma_channel_config c2 = dma_channel_get_default_config(rgb_chan_2);  // default configs
    channel_config_set_transfer_data_size(&c2, DMA_SIZE_16);              // 8-bit txfers
    channel_config_set_read_increment(&c2, true);                        // yes read incrementing
    channel_config_set_write_increment(&c2, false);                      // no write incrementing
    channel_config_set_dreq(&c2, DREQ_PIO0_TX3) ;                        // DREQ_PIO0_TX2 pacing (FIFO)
    channel_config_set_chain_to(&c2, rgb_chan_3);                        // chain to other channel

    dma_channel_configure(
        rgb_chan_2,                 // Channel to be configured
        &c2,                        // The configuration we just created
        &pio->txf[rgb_sm3],          // write address (RGB PIO TX FIFO)
        &vga_data_array,            // place holder for first load from channel one
        TXCOUNT,                    // Number of transfers; in this case each is 1 byte.
        true                       // Don't start immediately.
    );

    // ===============================================
    // Channel 3 (reconfigures the channel 0 for next frame)
    dma_channel_config c3 = dma_channel_get_default_config(rgb_chan_3);   // default configs
    channel_config_set_transfer_data_size(&c3, DMA_SIZE_32);              // 32-bit txfers
    channel_config_set_read_increment(&c3, false);                        // no read incrementing
    channel_config_set_write_increment(&c3, false);                       // no write incrementing
    channel_config_set_chain_to(&c3, rgb_chan_2);                         // chain to other channel

    dma_channel_configure(
        rgb_chan_3,                         // Channel to be configured
        &c3,                                // The configuration we just created
        &dma_hw->ch[rgb_chan_2].read_addr,  // Write address (channel 0 read address)
        &address_pointer,                    // Read address (POINTER TO AN ADDRESS)
        1,                                  // Number of transfers, in this case each is 4 byte
        true                               // Don't start immediately.
    );
    
}


// A function for drawing a pixel with a specified color.
// Note that because information is passed to the PIO state machines through
// a DMA channel, we only need to modify the contents of the array and the
// pixels will be automatically updated on the screen.
void drawPixel(short x, short y, unsigned short color) {
    if (x > 319) x = 319 ;
    if (x < 0) x = 0 ;
    if (y < 0) y = 0 ;
    if (y > 239) y = 239 ;
    // Which pixel is it?
    int pixel = ((320 * y ) + x) ;
    // signel byte/pixel
    vga_data_array[pixel] = color ;  
}

// fill a rectangle
void fillRect(short x, short y, short w, short h, unsigned short color) {
  for(int i=x; i<(x+w); i++) {
    for(int j=y; j<(y+h); j++) {
        drawPixel(i, j, color);
    }
  }
}