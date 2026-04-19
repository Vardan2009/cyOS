#include <stdio.h>

#include "vga.h"

void printi(int value) {
    if (value < 0) {
        VGAPrintC('-');
        value = -value;
    }

    char buffer[20];
    int i = 0;

    do {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);

    while (i > 0) VGAPrintC(buffer[--i]);
}

void printui(unsigned int value, int width, char padChar) {
    char buffer[20];
    int i = 0;

    do {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);

    while (i < width) buffer[i++] = padChar;

    while (i > 0) VGAPrintC(buffer[--i]);
}

void printhx(unsigned int value, int width, char padChar) {
    char buffer[20];
    int i = 0;

    if (value == 0) {
        buffer[i++] = '0';
    } else {
        while (value > 0) {
            int digit = value % 16;
            buffer[i++] = (digit < 10) ? (digit + '0') : (digit - 10 + 'a');
            value /= 16;
        }
    }

    while (i < width) buffer[i++] = padChar;

    while (i > 0) VGAPrintC(buffer[--i]);
}

void printfloat(float value, int width, int precision, char padChar) {
    if (value < 0) {
        VGAPrintC('-');
        value = -value;
    }
    int intPart = (int)value;
    float fracPart = value - intPart;

    printi(intPart);
    VGAPrintC('.');
    for (int i = 0; i < precision; i++) {
        fracPart *= 10;
        int digit = (int)fracPart;
        VGAPrintC(digit + '0');
        fracPart -= digit;
    }
}

void vprintf_impl(const char *format, va_list args) {
    for (const char *p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;

            char padChar = ' ';
            if (*p == '0') {
                padChar = '0';
                p++;
            }

            int width = 0;
            while (*p >= '0' && *p <= '9') {
                width = width * 10 + (*p - '0');
                p++;
            }

            switch (*p) {
                case 's': {
                    char *str = va_arg(args, char *);
                    VGAPrint(str);
                    break;
                }
                case 'd': {
                    int num = va_arg(args, int);
                    printi(num);
                    break;
                }
                case 'u': {
                    unsigned int num = va_arg(args, unsigned int);
                    printui(num, width, padChar);
                    break;
                }
                case 'c': {
                    int num = va_arg(args, int);
                    VGAPrintC((char)num);
                    break;
                }
                case 'x': {
                    unsigned int num = va_arg(args, unsigned int);
                    printhx(num, width, padChar);
                    break;
                }
                case 'f': {
                    float num = va_arg(args, double);
                    int precision = 6;
                    printfloat(num, width, precision, padChar);
                    break;
                }
                default:
                    VGAPrintC('%');
                    VGAPrintC(*p);
                    break;
            }
        } else {
            VGAPrintC(*p);
        }
    }
}

void vprintf(const char *format, va_list args) { vprintf_impl(format, args); }

void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf_impl(format, args);
    va_end(args);
}
