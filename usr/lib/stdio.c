#include "stdio.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "syscall.h"

void puts(const char *s) { write(FD_STDOUT, s, strlen(s)); }

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

typedef struct {
    char *buf;
    int max;
    int pos;
    int fd;
    int is_buffer;
} out_t;

static void out_char(out_t *o, char c) {
    if (o->is_buffer) {
        if (o->pos < o->max - 1) {
            o->buf[o->pos] = c;
        }
        o->pos++;
    } else {
        write(o->fd, &c, 1);
        o->pos++;
    }
}

static void out_str(out_t *o, const char *s) {
    while (*s) out_char(o, *s++);
}

static void out_int(out_t *o, int n, int base, int pad, char pad_char) {
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
    while (len++ < pad) out_char(o, pad_char);

    out_str(o, &buf[i + 1]);
}

int vformat(out_t *o, const char *fmt, va_list args) {
    while (*fmt) {
        if (*fmt != '%') {
            out_char(o, *fmt++);
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
                out_int(o, va_arg(args, int), 10, width, pad_char);
                break;

            case 'u':
                out_int(o, (int)va_arg(args, unsigned int), 10, width,
                        pad_char);
                break;

            case 'x':
                out_int(o, (int)va_arg(args, unsigned int), 16, width,
                        pad_char);
                break;

            case 's': {
                const char *s = va_arg(args, const char *);
                if (!s) s = "(null)";
                int len = strlen(s);
                while (len++ < width) out_char(o, pad_char);
                out_str(o, s);
                break;
            }

            case 'c':
                out_char(o, (char)va_arg(args, int));
                break;

            case '%':
                out_char(o, '%');
                break;

            default:
                out_char(o, '?');
                break;
        }
    }
    return o->pos;
}

int vfprintf(int fd, const char *fmt, va_list args) {
    out_t o = {.buf = NULL, .max = 0, .pos = 0, .fd = fd, .is_buffer = 0};
    return vformat(&o, fmt, args);
}

int fprintf(int fd, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int r = vfprintf(fd, fmt, args);
    va_end(args);
    return r;
}

int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int r = vfprintf(FD_STDOUT, fmt, args);
    va_end(args);
    return r;
}

int vsnprintf(char *buf, int size, const char *fmt, va_list args) {
    out_t o = {.buf = buf, .max = size, .pos = 0, .fd = 0, .is_buffer = 1};

    int written = vformat(&o, fmt, args);

    if (size > 0) {
        int term = (o.pos < size) ? o.pos : size - 1;
        buf[term] = '\0';
    }

    return written;
}

int snprintf(char *buf, int size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int r = vsnprintf(buf, size, fmt, args);
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
