#include "fd.h"

#include <string.h>

#include "process.h"

int FDAlloc(Process *proc) {
    for (int i = 3; i < FD_MAX; i++)
        if (proc->fds[i].type == FD_TYPE_NONE) return i;
    return -1;
}

void FDInitProcess(Process *proc) {
    memset(proc->fds, 0, sizeof(proc->fds));

    extern void FDSetupTTY(FileDescriptor * fd, int readable, int writable);

    FDSetupTTY(&proc->fds[FD_STDIN], 1, 0);
    FDSetupTTY(&proc->fds[FD_STDOUT], 0, 1);
    FDSetupTTY(&proc->fds[FD_STDERR], 0, 1);
}

void FDInherit(Process *child, Process *parent) {
    memcpy(child->fds, parent->fds, sizeof(parent->fds));
    // NOTE: refcounting wouldve been better
    // but its fine for now idc
}

int FDRead(Process *proc, int fdNum, void *buf, uint32_t count) {
    if (fdNum < 0 || fdNum >= FD_MAX) return -1;
    FileDescriptor *fd = &proc->fds[fdNum];
    if (fd->type == FD_TYPE_NONE || !fd->readable) return -1;
    if (!fd->ops.read) return -1;
    return fd->ops.read(fd, buf, count);
}

int FDWrite(Process *proc, int fdNum, const void *buf, uint32_t count) {
    if (fdNum < 0 || fdNum >= FD_MAX) return -1;
    FileDescriptor *fd = &proc->fds[fdNum];
    if (fd->type == FD_TYPE_NONE || !fd->writable) return -1;
    if (!fd->ops.write) return -1;
    return fd->ops.write(fd, buf, count);
}

int FDClose(Process *proc, int fdNum) {
    if (fdNum < 3 || fdNum >= FD_MAX) return -1;
    FileDescriptor *fd = &proc->fds[fdNum];
    if (fd->type == FD_TYPE_NONE) return -1;
    if (fd->ops.close) fd->ops.close(fd);
    fd->type = FD_TYPE_NONE;
    return 0;
}
