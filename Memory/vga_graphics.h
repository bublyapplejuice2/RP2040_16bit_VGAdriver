/**
 * Hunter Adams (vha3@cornell.edu)
 * 
 *
 * HARDWARE CONNECTIONS
 *  - GPIO 16 ---> VGA Hsync
 *  - GPIO 17 ---> VGA Vsync
 *  - RP2040 GND ---> VGA GND
 *
 * RESOURCES USED
 *  - PIO state machines 0, 1, and 2 on PIO instance 0
 *  - DMA channels 0, 1, 2, and 3
 *  - 153.6 kBytes of RAM (for pixel color data)
 *
 * NOTE
 *  - This is a translation of the display primitives
 *    for the PIC32 written by Bruce Land and students
 *
 */


// Give the I/O pins that we're using some names that make sense - usable in main()
enum vga_pins {HSYNC=27, VSYNC, RED_PIN, GREEN_PIN, BLUE_PIN} ;

// We can only produce 8 (3-bit) colors, so let's give them readable names - usable in main()
enum colors {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE} ;

// VGA primitives - usable in main
void initVGA(void) ;