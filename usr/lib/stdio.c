#include "stdio.h"

#include <stdlib.h>
#include <string.h>

#include "syscall.h"

void puts(const char *s) {
    uint32_t len = 0;
    while (s[len]) len++;
    write(FD_STDOUT, s, len);
}

char *gets(char *buf, int size) {
    int n = read(FD_STDIN, buf, size - 1);
    if (n <= 0) {
        buf[0] = '\0';
        return buf;
    }

    if (buf[n - 1] == '\n') n--;
    buf[n] = '\0';
    return buf;
}

static void fd_write_char(int fd, char c) { write(fd, &c, 1); }

static void fd_write_str(int fd, const char *s) { write(fd, s, strlen(s)); }

static void fd_write_int(int fd, int n, int base, int pad, char pad_char) {
    char buf[32];
    int i = 30;
    int neg = (base == 10 && n < 0);
    unsigned int u = neg ? (unsigned int)-n : (unsigned int)n;

    buf[31] = '\0';
    if (u == 0) {
        buf[i--] = '0';
    } else {
        while (u) {
            buf[i--] = "0123456789abcdef"[u % base];
            u /= base;
        }
    }
    if (neg) buf[i--] = '-';

    int len = 30 - i;
    while (len++ < pad) fd_write_char(fd, pad_char);
    fd_write_str(fd, &buf[i + 1]);
}

int vfprintf(int fd, const char *fmt, va_list args) {
    int written = 0;

    while (*fmt) {
        if (*fmt != '%') {
            fd_write_char(fd, *fmt++);
            written++;
            continue;
        }
        fmt++;

        char pad_char = ' ';
        if (*fmt == '0') {
            pad_char = '0';
            fmt++;
        }

        int width = 0;
        while (*fmt >= '0' && *fmt <= '9') width = width * 10 + (*fmt++ - '0');

        switch (*fmt++) {
            case 'd':
            case 'i':
                fd_write_int(fd, va_arg(args, int), 10, width, pad_char);
                break;
            case 'u':
                fd_write_int(fd, (int)va_arg(args, unsigned int), 10, width,
                             pad_char);
                break;
            case 'x':
                fd_write_int(fd, (int)va_arg(args, unsigned int), 16, width,
                             pad_char);
                break;
            case 's': {
                const char *s = va_arg(args, const char *);
                if (!s) s = "(null)";
                int len = strlen(s);
                while (len++ < width) fd_write_char(fd, pad_char);
                fd_write_str(fd, s);
                break;
            }
            case 'c':
                fd_write_char(fd, (char)va_arg(args, int));
                break;
            case '%':
                fd_write_char(fd, '%');
                break;
            default:
                fd_write_char(fd, '?');
                break;
        }
    }
    return written;
}

int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int r = vfprintf(FD_STDOUT, fmt, args);
    va_end(args);
    return r;
}

int fprintf(int fd, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int r = vfprintf(fd, fmt, args);
    va_end(args);
    return r;
}

int scanf(const char *fmt, ...) {
    char buf[256];
    gets(buf, sizeof(buf));

    va_list args;
    va_start(args, fmt);
    const char *p = buf;

    while (*fmt) {
        if (*fmt != '%') {
            fmt++;
            continue;
        }
        fmt++;
        switch (*fmt++) {
            case 'd': {
                while (*p == ' ') p++;
                *va_arg(args, int *) = atoi(p);
                while (*p && *p != ' ') p++;
                break;
            }
            case 's': {
                while (*p == ' ') p++;
                char *dst = va_arg(args, char *);
                while (*p && *p != ' ') *dst++ = *p++;
                *dst = '\0';
                break;
            }
        }
    }
    va_end(args);
    return 0;
}
