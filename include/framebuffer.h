// framebuffer.h -- Brad Slayter

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "lib/common.h"

/* Control characters for the console */
#define FG_RED 0b1111100000000000
#define FG_GREEN 0b0000011111100000
#define FG_BLUE 0b0000000000011111
#define FG_YELLOW 0b1111111111100000
#define FG_MAGENTA 0b1111100000011111
#define FG_CYAN 0b0000011111111111
#define FG_WHITE 0b1111111111111111
#define FG_BLACK 0b0000000000000000
#define FG_HALF 0b0111101111101111 // color >> 1 & FG_HALF

#define COLOUR_PUSH "\013"
#define COLOUR_POP "\014"

#define BG_RED 0b1111100000000000
#define BG_GREEN 0b0000011111100000
#define BG_BLUE 0b0000000000011111
#define BG_YELLOW 0b1111111111100000
#define BG_MAGENTA 0b1111100000011111
#define BG_CYAN 0b0000011111111111
#define BG_WHITE 0b1111111111111111
#define BG_BLACK 0b0000000000000000
#define BG_HALF 0b0111101111101111 // color >> 1 & BG_HALF

void fb_init();
void drawStuff();

void monPut(char c);
void monPuts(const char *str);
void monPutHex(uint32_t n);
void monPutDec(int n);
void monClear();
void monSetColor(unsigned int bg, unsigned int fg);

#endif