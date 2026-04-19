#ifndef CY_VGA_H
#define CY_VGA_H

#include <stdint.h>

#define COLOR8_BLACK 0
#define COLOR8_LIGHT_GRAY 7

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define VGA_COLOR(FG, BG) ((BG) << 8 | (FG << 12))
#define VGA_CHAR(CH, COL) ((CH) | (COL))

void VGAPrint(const char *s);
void VGAScroll();

void VGALineBreak();
void VGAReset();

#endif  // CY_VGA_H
