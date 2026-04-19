#ifndef CY_STDIO_H
#define CY_STDIO_H

#include <stdarg.h>

void printi(int value);
void printui(unsigned int value, int width, char padChar);
void printhx(unsigned int value, int width, char padChar);
void printf(const char *fmt, ...);
void vprintf(const char *fmt, va_list);
void printfloat(float value, int width, int precision, char padChar);

void scanl(char *buffer, unsigned int size);
int scanf(const char *fmt, ...);

#endif  // CY_STDIO_H
