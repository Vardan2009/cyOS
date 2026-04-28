#include "vga.h"

#include <io.h>
#include <stdint.h>

static uint16_t column = 0;
static uint16_t line = 0;
static uint16_t *const vga = (uint16_t *const)0xC00B8000;
static const uint16_t defaultColor = VGA_COLOR(COLOR8_BLACK, COLOR8_LIGHT_GRAY);
static uint16_t currentColor = defaultColor;

void VGAReset() {
    line = 0;
    column = 0;
    currentColor = defaultColor;

    for (uint16_t y = 0; y < VGA_HEIGHT; ++y)
        for (uint16_t x = 0; x < VGA_WIDTH; ++x)
            vga[y * VGA_WIDTH + x] = VGA_CHAR(' ', defaultColor);

    VGASetCurPos(0);
}

void VGALineBreak() {
    if (line < VGA_HEIGHT - 1)
        ++line;
    else
        VGAScroll();
    column = 0;

    VGASetCurPos(line * VGA_WIDTH + column);
}

void VGAScroll() {
    for (uint16_t y = 0; y < VGA_HEIGHT; ++y)
        for (uint16_t x = 0; x < VGA_WIDTH; ++x)
            vga[(y - 1) * VGA_WIDTH + x] = vga[y * VGA_WIDTH + x];

    for (uint16_t x = 0; x < VGA_WIDTH; ++x)
        vga[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = VGA_CHAR(' ', defaultColor);
}

void VGABackspace() {
    if (column == 0) {
        column = VGA_WIDTH - 1;
        if (line > 0) --line;
    } else
        --column;
    VGAPrintC(' ');
    --column;

    VGASetCurPos(line * VGA_WIDTH + column);
}

void VGAPrintC(char c) {
    switch (c) {
        case '\n':
            VGALineBreak();
            break;
        case '\r':
            column = 0;
            break;
        case '\b':
            VGABackspace();
            break;
        case '\t':
            if (column == VGA_WIDTH) VGALineBreak();
            uint16_t tabl = 4 - (column % 4);
            while (tabl > 0) {
                vga[line * VGA_WIDTH + (column++)] =
                    VGA_CHAR(' ', defaultColor);
                --tabl;
            }
            break;
        default:
            if (column == VGA_WIDTH) VGALineBreak();
            vga[line * VGA_WIDTH + (column++)] = VGA_CHAR(c, currentColor);
    }

    VGASetCurPos(line * VGA_WIDTH + column);
}

void VGAPrint(char *s) {
    while (*s) VGAPrintC(*(s++));
}

void VGASetCurPos(uint16_t position) {
    outb(VGA_CTRL_REG, 0x0F);
    outb(VGA_DATA_REG, (uint8_t)(position & 0xFF));
    outb(VGA_CTRL_REG, 0x0E);
    outb(VGA_DATA_REG, (uint8_t)((position >> 8) & 0xFF));
}
