enum vga_pins {HSYNC=21, VSYNC} ;

// 8-bit color
#define WHITE   0b11111111
#define BLACK   0b00000000
#define RED     0b0000000000011111
#define GREEN   0b0000011111100000
#define BLUE    0b1111100000000000
#define CYAN    0b00011111
#define MAGENTA 0b11100011
#define YELLOW  0b11111100
#define GRAY1   0b00100100
#define GRAY2   0b01101101
#define GRAY3   0b10110110
#define GRAY4   0b11011010

// defining colors
typedef unsigned short ushort;
#define rgb(r,g,b) ( ((b)<<11) & BLUE | ((g)<<5) & GREEN | ((r)<<0) & RED )

// VGA primitives - usable in main
void initVGA(void) ;
void drawPixel(short x, short y, ushort color) ;
void drawPixelDither(short x, short y, ushort color1, ushort color2) ;
void drawVLine(short x, short y, short h, ushort color) ;
void drawHLine(short x, short y, short w, ushort color) ;
void drawLine(short x0, short y0, short x1, short y1, ushort color) ;
void drawRect(short x, short y, short w, short h, ushort color);
void drawRectDither(short x, short y, short w, short h, ushort color1, ushort color2);
void drawCircle(short x0, short y0, short r, ushort color) ;
void drawCircleHelper( short x0, short y0, short r, unsigned char cornername, ushort color) ;
void fillCircle(short x0, short y0, short r, ushort color) ;
void fillCircleHelper(short x0, short y0, short r, unsigned char cornername, short delta, ushort color) ;
void drawRoundRect(short x, short y, short w, short h, short r, ushort color) ;
void fillRoundRect(short x, short y, short w, short h, short r, ushort color) ;
void fillRect(short x, short y, short w, short h, ushort color) ;
void fillRectDither(short x, short y, short w, short h, ushort color1, ushort color2) ;
void drawChar(short x, short y, unsigned char c, ushort color, char bg, unsigned char size) ;
void setCursor(short x, short y);
void setTextColor(char c);
void setTextColor2(char c, char bg);
void setTextSize(unsigned char s);
void setTextWrap(char w);
void tft_write(unsigned char c) ;
void writeString(char* str) ;