#ifndef CY_FD_H
#define CY_FD_H

#include <stdint.h>

#define FD_MAX 16
#define FD_STDIN 0
#define FD_STDOUT 1
#define FD_STDERR 2

typedef enum {
    FD_TYPE_NONE = 0,
    FD_TYPE_TTY,
    FD_TYPE_FILE,
} FDType;

typedef struct FileDescriptor FileDescriptor;

typedef struct {
    int (*read)(FileDescriptor *fd, void *buf, uint32_t count);
    int (*write)(FileDescriptor *fd, const void *buf, uint32_t count);
    int (*close)(FileDescriptor *fd);

    // optional
    int (*seek)(FileDescriptor *fd, int32_t offset, int whence);
} FDOps;

struct FileDescriptor {
    FDType type;
    FDOps ops;
    uint8_t readable;
    uint8_t writable;

    // FIL *for files, NULL for other stuff
    void *data;
};

struct _Process;

int FDAlloc(struct _Process *proc);
void FDInitProcess(struct _Process *proc);
void FDInherit(struct _Process *child, struct _Process *parent);

int FDRead(struct _Process *proc, int fdNum, void *buf, uint32_t count);
int FDWrite(struct _Process *proc, int fdNum, const void *buf, uint32_t count);
int FDClose(struct _Process *proc, int fdNum);

#endif  // CY_FD_H
