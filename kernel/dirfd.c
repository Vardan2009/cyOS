#include <string.h>

#include "fatfs/ff.h"
#include "fd.h"
#include "kmalloc.h"

static int DirRead(FileDescriptor *fd, void *buf, uint32_t count) {
    FDDir *d = (FDDir *)fd->data;
    DirEnt *ent = (DirEnt *)buf;
    FILINFO fno;

    if (f_readdir(&d->dir, &fno) != FR_OK) return -1;
    if (fno.fname[0] == '\0') return 0;

    strncpy(ent->name, fno.fname, 255);
    ent->name[255] = '\0';
    ent->size = (uint32_t)fno.fsize;
    ent->isDir = (fno.fattrib & AM_DIR) ? 1 : 0;

    return 1;
}

static int DirClose(FileDescriptor *fd) {
    FDDir *d = (FDDir *)fd->data;
    f_closedir(&d->dir);
    kfree(d);
    fd->type = FD_TYPE_NONE;
    fd->data = NULL;
    return 0;
}

static FDOps dirOps = {
    .read = DirRead,
    .write = NULL,
    .close = DirClose,
    .seek = NULL,
};

int FDOpenDir(FileDescriptor *fd, const char *resolved) {
    FDDir *d = kmalloc(sizeof(FDDir));
    strncpy(d->path, resolved, PATH_MAX);

    if (f_opendir(&d->dir, resolved) != FR_OK) {
        kfree(d);
        return -1;
    }

    fd->type = FD_TYPE_DIR;
    fd->ops = dirOps;
    fd->readable = 1;
    fd->writable = 0;
    fd->data = d;

    return 0;
}
