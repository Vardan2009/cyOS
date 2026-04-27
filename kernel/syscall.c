#include "syscall.h"

#include <stdio.h>

#include "elf.h"
#include "fd.h"
#include "filefd.h"
#include "gdt.h"
#include "idt.h"
#include "kmalloc.h"
#include "memory.h"
#include "process.h"

extern void SyscallStub();

void SyscallInit() { IDTSetGate(0x80, (uint32_t)SyscallStub, 0x08, 0xEF); }

static void SysExit(uint32_t code) {
    Process *proc = currentProcess;
    if (!proc) return;

    proc->exitCode = code;

    uint32_t savedEbp = proc->returnEbp;
    uint32_t savedKstack = proc->kernelStack;
    Process *parent = proc->parent;

    ProcessFreePages(proc);
    kfree((void *)(savedKstack - 0x2000));

    for (int i = 0; i < proc->envc; ++i) kfree(proc->envp[i]);

    currentProcess = parent;

    if (parent == NULL) {
        asm volatile("mov %0, %%cr3" ::"r"((uint32_t)initialPageDir -
                                           (uint32_t)KERNEL_START));
        asm volatile(
            "movw $0x10, %%ax \n"
            "movw %%ax, %%ds  \n"
            "movw %%ax, %%es  \n"
            "movw %%ax, %%fs  \n"
            "movw %%ax, %%gs  \n" ::
                : "eax");
    } else {
        asm volatile("mov %0, %%cr3" ::"r"(parent->pageDir));
        TSSSetKernelStack(parent->kernelStack);
        asm volatile(
            "movw $0x10, %%ax \n"
            "movw %%ax, %%ds  \n"
            "movw %%ax, %%es  \n"
            "movw %%ax, %%fs  \n"
            "movw %%ax, %%gs  \n" ::
                : "eax");
    }

    asm volatile(
        "mov %0, %%ebp  \n"
        "leave          \n"
        "ret            \n" ::"r"(savedEbp));
}

#include <string.h>

static uint32_t SysExec(const char *path, char **userargv) {
    if ((uint32_t)path >= KERNEL_START) return (uint32_t)-1;

    Process *child = ELFLoad(path);
    if (!child) return (uint32_t)-1;

    if (userargv) {
        int i = 0;
        while (i < PROCESS_MAX_ARGS && userargv[i]) {
            if ((uint32_t)userargv[i] >= KERNEL_START) break;
            char *pstrdup = kmalloc(strlen(userargv[i]) + 1);
            memcpy(pstrdup, userargv[i], strlen(userargv[i]) + 1);
            child->argv[i] = pstrdup;
            ++i;
        }
        child->argc = i;
    }

    Process *parent = currentProcess;
    uint32_t parentReturn = parent ? parent->returnEbp : 0;

    ProcessExecute(child);

    if (parent) {
        currentProcess = parent;
        parent->returnEbp = parentReturn;
        asm volatile("mov %0, %%cr3" ::"r"(parent->pageDir));
        TSSSetKernelStack(parent->kernelStack);
    }

    return 0;
}

static int SysOpen(const char *path, uint32_t flags) {
    if ((uint32_t)path >= KERNEL_START) return -1;

    int slot = FDAlloc(currentProcess);
    if (slot < 0) return -1;

    if (FDOpenFile(&currentProcess->fds[slot], path, (uint8_t)flags) < 0)
        return -1;

    return slot;
}

#include "env.h"

static uint32_t SysGetEnv(const char *key, char *buf, uint32_t bufsz) {
    if ((uint32_t)key >= KERNEL_START) return 0;
    if ((uint32_t)buf >= KERNEL_START) return 0;

    char *val = EnvGet(currentProcess, key);
    if (!val) return 0;

    uint32_t len = strlen(val);
    if (len >= bufsz) len = bufsz - 1;
    memcpy(buf, val, len);
    buf[len] = '\0';
    return len;
}

static uint32_t SysSetEnv(const char *key, const char *value) {
    if ((uint32_t)key >= KERNEL_START) return (uint32_t)-1;
    if ((uint32_t)value >= KERNEL_START) return (uint32_t)-1;
    EnvSet(currentProcess, key, value);
    return 0;
}

static uint32_t SysUnsetEnv(const char *key) {
    if ((uint32_t)key >= KERNEL_START) return (uint32_t)-1;
    EnvUnset(currentProcess, key);
    return 0;
}

uint32_t SyscallDispatch(SyscallRegs *regs) {
    switch (regs->eax) {
        case SYSCALL_EXIT:
            SysExit(regs->ebx);
            return 0;

        case SYSCALL_EXEC:
            return SysExec((const char *)regs->ebx, (char **)regs->ecx);

        case SYSCALL_READ:
            return (uint32_t)FDRead(currentProcess, (int)regs->ebx,
                                    (void *)regs->ecx, regs->edx);
        case SYSCALL_WRITE:
            return (uint32_t)FDWrite(currentProcess, (int)regs->ebx,
                                     (const void *)regs->ecx, regs->edx);
        case SYSCALL_OPEN:
            return (uint32_t)SysOpen((const char *)regs->ebx, regs->ecx);

        case SYSCALL_CLOSE:
            return (uint32_t)FDClose(currentProcess, (int)regs->ebx);

        case SYSCALL_SEEK:
            if (regs->ebx >= FD_MAX) return (uint32_t)-1;
            FileDescriptor *fd = &currentProcess->fds[regs->ebx];
            if (!fd->ops.seek) return (uint32_t)-1;
            return (uint32_t)fd->ops.seek(fd, (int32_t)regs->ecx,
                                          (int)regs->edx);

        case SYSCALL_GETENV:
            return SysGetEnv((const char *)regs->ebx, (char *)regs->ecx,
                             regs->edx);
        case SYSCALL_SETENV:
            return SysSetEnv((const char *)regs->ebx, (const char *)regs->ecx);
        case SYSCALL_UNSETENV:
            return SysUnsetEnv((const char *)regs->ebx);

        default:
            printf("Unknown syscall %u\n", regs->eax);
            return (uint32_t)-1;
    }
}
