#include "ansi.h"

#include <stdint.h>
#include <string.h>

#include "vga.h"

typedef enum {
    S_NORMAL,
    S_ESC,
    S_CSI,
    S_OSC,
} ANSIState;

#define MAX_PARAMS 8

typedef struct {
    ANSIState state;
    int params[MAX_PARAMS];
    int param_count;
    int curParam;
    char priv;
} ANSIParser;

static ANSIParser p;

void ANSIInit(void) {
    memset(&p, 0, sizeof(p));
    p.state = S_NORMAL;
}

static int param(int i, int def) {
    if (i >= p.param_count) return def;
    return p.params[i] != 0 ? p.params[i] : def;
}

static const uint8_t ansiFGtoVGA[8] = {
    COLOR8_BLACK, COLOR8_RED,     COLOR8_GREEN, COLOR8_BROWN,
    COLOR8_BLUE,  COLOR8_MAGENTA, COLOR8_CYAN,  COLOR8_LIGHT_GRAY};

static const uint8_t ansiFGBrighttoVGA[8] = {
    COLOR8_DARK_GRAY,   COLOR8_LIGHT_RED,  COLOR8_LIGHT_GREEN,
    COLOR8_LIGHT_BROWN, COLOR8_LIGHT_BLUE, COLOR8_LIGHT_MAGENTA,
    COLOR8_LIGHT_CYAN,  COLOR8_WHITE};

static void ApplySGR(void) {
    if (p.param_count == 0) {
        VGASetColor(VGA_COLOR(COLOR8_BLACK, COLOR8_LIGHT_GRAY));
        return;
    }

    for (int i = 0; i < p.param_count; i++) {
        int n = p.params[i];

        if (n == 0) {
            VGASetColor(VGA_COLOR(COLOR8_BLACK, COLOR8_LIGHT_GRAY));

        } else if (n == 1) {
            uint16_t col = VGAGetColor();
            uint8_t fg = VGA_DISP_FG(col) | 0x08;
            uint8_t bg = VGA_DISP_BG(col);
            VGASetColor(VGA_COLOR(bg, fg));

        } else if (n == 2 || n == 22) {
            uint16_t col = VGAGetColor();
            uint8_t fg = VGA_DISP_FG(col) & ~0x08;
            uint8_t bg = VGA_DISP_BG(col);
            VGASetColor(VGA_COLOR(bg, fg));

        } else if (n == 7) {
            uint16_t col = VGAGetColor();
            uint8_t fg = VGA_DISP_FG(col);
            uint8_t bg = VGA_DISP_BG(col);
            VGASetColor(VGA_COLOR(fg, bg));

        } else if (n == 27) {
            VGASetColor(VGA_COLOR(COLOR8_BLACK, COLOR8_LIGHT_GRAY));

        } else if (n >= 30 && n <= 37) {
            uint16_t col = VGAGetColor();
            uint8_t bg = VGA_DISP_BG(col);
            uint8_t bright = VGA_DISP_FG(col) & 0x08;  // preserve bold
            VGASetColor(VGA_COLOR(bg, ansiFGtoVGA[n - 30] | bright));

        } else if (n == 39) {
            uint16_t col = VGAGetColor();
            uint8_t bg = VGA_DISP_BG(col);
            VGASetColor(VGA_COLOR(bg, COLOR8_LIGHT_GRAY));

        } else if (n >= 40 && n <= 47) {
            uint16_t col = VGAGetColor();
            uint8_t fg = VGA_DISP_FG(col);
            VGASetColor(VGA_COLOR(ansiFGtoVGA[n - 40], fg));

        } else if (n == 49) {
            uint16_t col = VGAGetColor();
            uint8_t fg = VGA_DISP_FG(col);
            VGASetColor(VGA_COLOR(COLOR8_BLACK, fg));

        } else if (n >= 90 && n <= 97) {
            uint16_t col = VGAGetColor();
            uint8_t bg = VGA_DISP_BG(col);
            VGASetColor(VGA_COLOR(bg, ansiFGBrighttoVGA[n - 90]));

        } else if (n >= 100 && n <= 107) {
            uint16_t col = VGAGetColor();
            uint8_t fg = VGA_DISP_FG(col);
            VGASetColor(VGA_COLOR(ansiFGBrighttoVGA[n - 100], fg));
        }
    }
}

static void DispatchCSI(char final) {
    int row, col;
    VGAGetCursor(&row, &col);

    switch (final) {
        case 'A':
            VGASetCursor(row - param(0, 1), col);
            break;

        case 'B':
            VGASetCursor(row + param(0, 1), col);
            break;

        case 'C':
            VGASetCursor(row, col + param(0, 1));
            break;

        case 'D':
            VGASetCursor(row, col - param(0, 1));
            break;

        case 'E':
            VGASetCursor(row + param(0, 1), 0);
            break;

        case 'F':
            VGASetCursor(row - param(0, 1), 0);
            break;

        case 'G':
            VGASetCursor(row, param(0, 1) - 1);
            break;

        case 'H':
        case 'f':
            VGASetCursor(param(0, 1) - 1, param(1, 1) - 1);
            break;

        case 'd':
            VGASetCursor(param(0, 1) - 1, col);
            break;

        case 'J':
            switch (param(0, 0)) {
                case 0:
                    VGAClearToEOL();
                    for (int r = row + 1; r < VGA_HEIGHT; r++) VGAClearLine(r);
                    VGASetCursor(row, col);
                    break;
                case 1:
                    for (int r = 0; r < row; r++) VGAClearLine(r);
                    VGAClearFromBOL();
                    VGASetCursor(row, col);
                    break;
                case 2:
                case 3:
                    VGAClearScreen();
                    VGASetCursor(0, 0);
                    break;
            }
            break;

        case 'K':
            switch (param(0, 0)) {
                case 0:
                    VGAClearToEOL();
                    VGASetCursor(row, col);
                    break;
                case 1:
                    VGAClearFromBOL();
                    VGASetCursor(row, col);
                    break;
                case 2:
                    VGAClearLine(row);
                    VGASetCursor(row, 0);
                    break;
            }
            break;

        case 'P':
            VGADeleteChars(param(0, 1));
            break;

        case 'S':
            for (int i = 0; i < param(0, 1); i++) VGAScroll();
            break;

        case 'm':
            ApplySGR();
            break;

        case 'h':
            if (p.priv == '?') {
                switch (param(0, 0)) {
                    case 25:
                        VGAShowCursor();
                        break;
                }
            }
            break;

        case 'l':
            if (p.priv == '?') {
                switch (param(0, 0)) {
                    case 25:
                        VGAHideCursor();
                        break;
                }
            }
            break;
    }
}

void ANSIPutC(char c) {
    switch (p.state) {
        case S_NORMAL:
            if (c == '\x1B') {
                p.state = S_ESC;
            } else {
                VGAPrintC(c);
            }
            break;

        case S_ESC:
            if (c == '[') {
                for (int i = 0; i < MAX_PARAMS; i++) p.params[i] = 0;
                p.param_count = 0;
                p.curParam = 0;
                p.priv = 0;
                p.state = S_CSI;
            } else if (c == ']') {
                p.state = S_OSC;
            } else if (c == 'c') {
                VGAReset();
                p.state = S_NORMAL;
            } else if (c == 'M') {
                int row, col;
                VGAGetCursor(&row, &col);
                if (row > 0) VGASetCursor(row - 1, col);
                p.state = S_NORMAL;
            } else {
                p.state = S_NORMAL;
            }
            break;

        case S_CSI:
            if (c == '?' || c == '>' || c == '!') {
                p.priv = c;
            } else if (c >= '0' && c <= '9') {
                p.curParam = p.curParam * 10 + (c - '0');
            } else if (c == ';') {
                if (p.param_count < MAX_PARAMS)
                    p.params[p.param_count++] = p.curParam;
                p.curParam = 0;
            } else if (c >= 0x40 && c <= 0x7E) {
                if (p.param_count < MAX_PARAMS)
                    p.params[p.param_count++] = p.curParam;
                DispatchCSI(c);
                p.state = S_NORMAL;
            }
            break;

        case S_OSC:
            if (c == '\x07' || c == '\x1B') p.state = S_NORMAL;
            break;
    }
}

void ANSIPutS(char *s) {
    while (*s) ANSIPutC(*s++);
}
