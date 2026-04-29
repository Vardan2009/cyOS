#ifndef CYLIBC_DIRENT_H
#define CYLIBC_DIRENT_H

#include <stdint.h>
#include <stdlib.h>

#include "syscall.h"

typedef struct {
    char name[256];
    uint32_t size;
    uint8_t isDir;
} DirEnt;

typedef struct {
    int fd;
    DirEnt ent;
} DIR;

static inline DIR *opendir(const char *path) {
    int fd = (int)syscall1(SYSCALL_OPENDIR, (uint32_t)path);
    if (fd < 0) return NULL;

    DIR *d = malloc(sizeof(DIR));
    d->fd = fd;
    return d;
}

static inline DirEnt *readdir(DIR *d) {
    int r = read(d->fd, &d->ent, sizeof(DirEnt));
    return r > 0 ? &d->ent : NULL;
}

static inline void closedir(DIR *d) {
    close(d->fd);
    free(d);
}

#endif  // CYLIBC_DIRENT_H
