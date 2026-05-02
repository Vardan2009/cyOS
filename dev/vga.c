#include "vga.h"

#include <io.h>
#include <stdint.h>

static uint16_t column = 0;
static uint16_t line = 0;
static uint16_t *const vga = (uint16_t *const)0xC00B8000;
static const uint16_t defaultColor = VGA_COLOR(COLOR8_BLACK, COLOR8_LIGHT_GRAY);
static uint16_t currentColor = VGA_COLOR(COLOR8_BLACK, COLOR8_LIGHT_GRAY);

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
    for (uint16_t y = 1; y < VGA_HEIGHT; ++y)
        for (uint16_t x = 0; x < VGA_WIDTH; ++x)
            vga[(y - 1) * VGA_WIDTH + x] = vga[y * VGA_WIDTH + x];

    for (uint16_t x = 0; x < VGA_WIDTH; ++x)
        vga[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = VGA_CHAR(' ', defaultColor);
}

void VGABackspace() {
    if (column == 0) {
        column = VGA_WIDTH - 1;
        if (line > 0) --line;
    } else {
        --column;
    }
    vga[line * VGA_WIDTH + column] = VGA_CHAR(' ', currentColor);
    VGASetCurPos(line * VGA_WIDTH + column);
}

void VGAPrintC(char c) {
    switch (c) {
        case '\n':
            VGALineBreak();
            break;
        case '\r':
            column = 0;
            VGASetCurPos(line * VGA_WIDTH + column);
            break;
        case '\b':
            VGABackspace();
            break;
        case '\t': {
            if (column == VGA_WIDTH) VGALineBreak();
            uint16_t spaces = 4 - (column % 4);
            while (spaces-- > 0) {
                vga[line * VGA_WIDTH + column++] = VGA_CHAR(' ', currentColor);
                if (column == VGA_WIDTH) {
                    VGALineBreak();
                    break;
                }
            }
            VGASetCurPos(line * VGA_WIDTH + column);
            break;
        }
        default:
            if (column == VGA_WIDTH) VGALineBreak();
            vga[line * VGA_WIDTH + column++] = VGA_CHAR(c, currentColor);
            VGASetCurPos(line * VGA_WIDTH + column);
    }
}

void VGAPrint(char *s) {
    while (*s) VGAPrintC(*s++);
}

void VGASetCurPos(uint16_t position) {
    outb(VGA_CTRL_REG, 0x0F);
    outb(VGA_DATA_REG, (uint8_t)(position & 0xFF));
    outb(VGA_CTRL_REG, 0x0E);
    outb(VGA_DATA_REG, (uint8_t)((position >> 8) & 0xFF));
}

uint16_t VGAGetColor(void) { return currentColor; }

void VGASetColor(uint16_t color) { currentColor = color; }

void VGAGetCursor(int *row, int *col) {
    *row = (int)line;
    *col = (int)column;
}

void VGASetCursor(int row, int col) {
    if (row < 0) row = 0;
    if (row >= VGA_HEIGHT) row = VGA_HEIGHT - 1;
    if (col < 0) col = 0;
    if (col >= VGA_WIDTH) col = VGA_WIDTH - 1;
    line = (uint16_t)row;
    column = (uint16_t)col;
    VGASetCurPos(line * VGA_WIDTH + column);
}

void VGAClearScreen(void) {
    for (uint16_t y = 0; y < VGA_HEIGHT; ++y)
        for (uint16_t x = 0; x < VGA_WIDTH; ++x)
            vga[y * VGA_WIDTH + x] = VGA_CHAR(' ', currentColor);
}

void VGAClearLine(int row) {
    if (row < 0 || row >= VGA_HEIGHT) return;
    for (uint16_t x = 0; x < VGA_WIDTH; ++x)
        vga[row * VGA_WIDTH + x] = VGA_CHAR(' ', currentColor);
}

void VGAClearToEOL(void) {
    for (uint16_t x = column; x < VGA_WIDTH; ++x)
        vga[line * VGA_WIDTH + x] = VGA_CHAR(' ', currentColor);
}

void VGAClearFromBOL(void) {
    for (uint16_t x = 0; x <= column; ++x)
        vga[line * VGA_WIDTH + x] = VGA_CHAR(' ', currentColor);
}

void VGADeleteChars(int count) {
    if (count <= 0) return;
    for (int x = (int)column; x < VGA_WIDTH; ++x) {
        int src = x + count;
        vga[line * VGA_WIDTH + x] = (src < VGA_WIDTH)
                                        ? vga[line * VGA_WIDTH + src]
                                        : VGA_CHAR(' ', currentColor);
    }
}

void VGAShowCursor(void) {
    outb(VGA_CTRL_REG, 0x0A);
    outb(VGA_DATA_REG, (inb(VGA_DATA_REG) & 0xC0) | 14);
    outb(VGA_CTRL_REG, 0x0B);
    outb(VGA_DATA_REG, (inb(VGA_DATA_REG) & 0xE0) | 15);
}

void VGAHideCursor(void) {
    outb(VGA_CTRL_REG, 0x0A);
    outb(VGA_DATA_REG, 0x20);
}
