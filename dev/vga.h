#ifndef CY_VGA_H
#define CY_VGA_H

#include <stdint.h>

#define COLOR8_BLACK 0
#define COLOR8_BLUE 1
#define COLOR8_GREEN 2
#define COLOR8_CYAN 3
#define COLOR8_RED 4
#define COLOR8_MAGENTA 5
#define COLOR8_BROWN 6
#define COLOR8_LIGHT_GRAY 7
#define COLOR8_DARK_GRAY 8
#define COLOR8_LIGHT_BLUE 9
#define COLOR8_LIGHT_GREEN 10
#define COLOR8_LIGHT_CYAN 11
#define COLOR8_LIGHT_RED 12
#define COLOR8_LIGHT_MAGENTA 13
#define COLOR8_LIGHT_BROWN 14
#define COLOR8_WHITE 15

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define VGA_CTRL_REG 0x3D4
#define VGA_DATA_REG 0x3D5

#define VGA_COLOR(FG, BG) ((BG) << 8 | (FG << 12))
#define VGA_CHAR(CH, COL) ((CH) | (COL))

void VGAPrintC(char c);
void VGAPrint(const char *s);
void VGAScroll();
void VGABackspace();

void VGALineBreak();
void VGAReset();

void VGASetCurPos(uint16_t position);

#endif  // CY_VGA_H
