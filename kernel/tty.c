#include <string.h>

#include "console.h"
#include "fd.h"
#include "inbuffer.h"

#define TTY_BUFSZ 256

typedef struct {
    char buf[TTY_BUFSZ];
    uint32_t len;
} TTYLine;

static TTYLine ttyLine;

static int TTYRead(FileDescriptor *fd, void *out, uint32_t count) {
    char *dst = (char *)out;
    uint32_t copied = 0;

    while (copied < count) {
        char c = InBufferGetC();

        switch (c) {
            case '\b':
                if (ttyLine.len > 0) {
                    ttyLine.len--;
                    ConsolePutC('\b');
                }
                break;

            case 21:
                while (ttyLine.len > 0) {
                    ttyLine.len--;
                    ConsolePutC('\b');
                }
                break;

            case '\n':
            case '\r':
                ConsolePutC('\n');
                ttyLine.buf[ttyLine.len++] = '\n';

                uint32_t to_copy = ttyLine.len < count ? ttyLine.len : count;
                memcpy(dst, ttyLine.buf, to_copy);
                copied = to_copy;
                ttyLine.len = 0;
                goto done;

            default:
                if (c >= 32 && ttyLine.len < TTY_BUFSZ - 1) {
                    ttyLine.buf[ttyLine.len++] = c;
                    ConsolePutC(c);
                }
                break;
        }
    }

done:
    return (int)copied;
}

static int TTYWrite(FileDescriptor *fd, const void *buf, uint32_t count) {
    const char *src = (const char *)buf;
    for (uint32_t i = 0; i < count; i++) ConsolePutC(src[i]);
    return (int)count;
}

static int TTYClose(FileDescriptor *fd) { return 0; }

static FDOps ttyOps = {
    .read = TTYRead,
    .write = TTYWrite,
    .close = TTYClose,
    .seek = NULL,
};

void FDSetupTTY(FileDescriptor *fd, int readable, int writable) {
    fd->type = FD_TYPE_TTY;
    fd->ops = ttyOps;
    fd->readable = readable;
    fd->writable = writable;
    fd->data = NULL;
}
