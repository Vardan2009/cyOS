#include "filefd.h"

#include "fatfs/ff.h"
#include "fd.h"
#include "kmalloc.h"

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

static int FileRead(FileDescriptor *fd, void *buf, uint32_t count) {
    FIL *fil = (FIL *)fd->data;
    UINT bytes_read;
    FRESULT res = f_read(fil, buf, count, &bytes_read);
    return res == FR_OK ? (int)bytes_read : -1;
}

static int FileWrite(FileDescriptor *fd, const void *buf, uint32_t count) {
    FIL *fil = (FIL *)fd->data;
    UINT bytes_written;
    FRESULT res = f_write(fil, buf, count, &bytes_written);
    return res == FR_OK ? (int)bytes_written : -1;
}

static int FileClose(FileDescriptor *fd) {
    FIL *fil = (FIL *)fd->data;
    FRESULT res = f_close(fil);
    kfree(fil);
    fd->data = NULL;
    fd->type = FD_TYPE_NONE;
    return res == FR_OK ? 0 : -1;
}

static int FileSeek(FileDescriptor *fd, int32_t offset, int whence) {
    FIL *fil = (FIL *)fd->data;
    FSIZE_t pos;

    switch (whence) {
        case SEEK_SET:
            pos = (FSIZE_t)offset;
            break;
        case SEEK_CUR:
            pos = f_tell(fil) + offset;
            break;
        case SEEK_END:
            pos = f_size(fil) + offset;
            break;
        default:
            return -1;
    }

    return f_lseek(fil, pos) == FR_OK ? 0 : -1;
}

static FDOps fileOps = {
    .read = FileRead,
    .write = FileWrite,
    .close = FileClose,
    .seek = FileSeek,
};

int FDOpenFile(FileDescriptor *fd, const char *path, uint8_t flags) {
    FIL *fil = kmalloc(sizeof(FIL));
    if (f_open(fil, path, flags) != FR_OK) {
        kfree(fil);
        return -1;
    }

    fd->type = FD_TYPE_FILE;
    fd->ops = fileOps;
    fd->readable = (flags & FA_READ) ? 1 : 0;
    fd->writable = (flags & FA_WRITE) ? 1 : 0;
    fd->data = fil;
    return 0;
}
