/**
 * Howard Hua (hth27@cornell.edu)
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
 *  - 240 kBytes of RAM (for pixel color data)
 *
 */


// Give the I/O pins that we're using some names that make sense - usable in main()
enum vga_pins {HSYNC=21, VSYNC} ;

#define RED     0b0000000000011111
#define GREEN   0b0000011111100000
#define BLUE    0b1111100000000000
#define rgb(r,g,b) ( ((b)<<11) & BLUE | ((g)<<5) & GREEN | ((r)<<0) & RED )

// VGA primitives - usable in main
void initVGA(void) ;
void drawPixel(short x, short y, unsigned short color) ;
void fillRect(short x, short y, short w, short h, unsigned short color) ;
void colorPixel();