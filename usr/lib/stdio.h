#ifndef CYLIBC_STDIO_H
#define CYLIBC_STDIO_H

#include <stdarg.h>

#define FD_STDIN 0
#define FD_STDOUT 1
#define FD_STDERR 2

#define FA_READ 0x01
#define FA_WRITE 0x02

#define O_RDONLY FA_READ
#define O_WRONLY FA_WRITE
#define O_RDWR (FA_READ | FA_WRITE)

#define O_CREAT FA_CREATE_ALWAYS

void puts(const char *s);
char *gets(char *buf, int size);

int vfprintf(int fd, const char *fmt, va_list args);
int fprintf(int fd, const char *fmt, ...);

int printf(const char *fmt, ...);

int vsnprintf(char *buf, int size, const char *fmt, va_list args);
int snprintf(char *buf, int size, const char *fmt, ...);

int scanf(const char *fmt, ...);

#endif  // CY_STDIO_H
