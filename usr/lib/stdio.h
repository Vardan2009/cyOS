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
int vfprintf(int fd, const char *fmt, va_list args);
int printf(const char *fmt, ...);
int fprintf(int fd, const char *fmt, ...);
int scanf(const char *fmt, ...);

#endif  // CYLIBC_STDIO_H
