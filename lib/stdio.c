#include <stdio.h>

#include "console.h"
#include "inbuffer.h"

void printi(int value) {
    if (value < 0) {
        ConsolePutC('-');
        value = -value;
    }

    char buffer[20];
    int i = 0;

    do {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);

    while (i > 0) ConsolePutC(buffer[--i]);
}

void printui(unsigned int value, int width, char padChar) {
    char buffer[20];
    int i = 0;

    do {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);

    while (i < width) buffer[i++] = padChar;

    while (i > 0) ConsolePutC(buffer[--i]);
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

    while (i > 0) ConsolePutC(buffer[--i]);
}

void printfloat(float value, int width, int precision, char padChar) {
    if (value < 0) {
        ConsolePutC('-');
        value = -value;
    }
    int intPart = (int)value;
    float fracPart = value - intPart;

    printi(intPart);
    ConsolePutC('.');
    for (int i = 0; i < precision; i++) {
        fracPart *= 10;
        int digit = (int)fracPart;
        ConsolePutC(digit + '0');
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
                    ConsolePutS(str);
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
                    ConsolePutC((char)num);
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
                    ConsolePutC('%');
                    ConsolePutC(*p);
                    break;
            }
        } else {
            ConsolePutC(*p);
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

void scanl(char *buffer, unsigned int size) {
    int i = 0;

    while (1) {
        char c = InBufferGetC();
        if (c == '\b') {
            if (i > 0) {
                if (buffer[i - 1] == '\t')
                    for (int i = 0; i < 4; ++i) ConsolePutC('\b');

                else
                    ConsolePutC('\b');
                i--;
            }
            continue;
        }
        if (c == '\0') continue;
        if (c == '\n' || i >= size - 1) {
            ConsolePutC('\n');
            break;
        }
        buffer[i++] = c;
        ConsolePutC(c);
    }
    buffer[i] = '\0';
}

int scanf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[256];
    scanl(buffer, 256);

    for (const char *p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 's': {
                    char *str = va_arg(args, char *);
                    int j = 0;
                    while (buffer[j] && buffer[j] != ' ' &&
                           j < sizeof(buffer) - 1) {
                        str[j] = buffer[j];
                        j++;
                    }
                    str[j] = '\0';
                    break;
                }
                case 'd': {
                    int *num = va_arg(args, int *);
                    *num = 0;
                    int sign = 1;
                    if (buffer[0] == '-') {
                        sign = -1;
                    }
                    for (int j = (sign == -1) ? 1 : 0;
                         buffer[j] >= '0' && buffer[j] <= '9'; j++) {
                        *num = *num * 10 + (buffer[j] - '0');
                    }
                    *num *= sign;
                    break;
                }
                case 'u': {
                    unsigned int *num = va_arg(args, unsigned int *);
                    *num = 0;
                    for (int j = 0; buffer[j] >= '0' && buffer[j] <= '9'; j++) {
                        *num = *num * 10 + (buffer[j] - '0');
                    }
                    break;
                }
                case 'c': {
                    char *ch = va_arg(args, char *);
                    *ch = buffer[0];
                    break;
                }
                default:
                    break;
            }
        }
    }

    va_end(args);
    return 1;
}
