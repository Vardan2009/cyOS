#ifndef CYLIBC_STDIO_H
#define CYLIBC_STDIO_H

#include <stdint.h>

#include "syscall.h"

#define FD_STDIN 0
#define FD_STDOUT 1
#define FD_STDERR 2

#define FA_READ 0x01
#define FA_WRITE 0x02

#define O_RDONLY FA_READ
#define O_WRONLY FA_WRITE
#define O_RDWR (FA_READ | FA_WRITE)
#define O_CREAT FA_CREATE_ALWAYS

static inline void puts(const char *s) {
    uint32_t len = 0;
    while (s[len]) len++;
    write(FD_STDOUT, s, len);
}

static inline char *gets(char *buf, int size) {
    int n = read(FD_STDIN, buf, size - 1);
    if (n <= 0) {
        buf[0] = '\0';
        return buf;
    }

    if (buf[n - 1] == '\n') n--;
    buf[n] = '\0';
    return buf;
}

#endif  // CYLIBC_STDIO_H
