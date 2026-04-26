#include "syscall.h"

#include <stdio.h>

#include "elf.h"
#include "gdt.h"
#include "idt.h"
#include "kmalloc.h"
#include "memory.h"
#include "process.h"
#include "vga.h"

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

static uint32_t SysExec(const char *path) {
    Process *proc = ELFLoad(path);
    if (!proc) return -1;

    ProcessExecute(proc);

    uint32_t ec = proc->exitCode;
    kfree(proc);

    return ec;
}

static void SysWrite(const char *text) { VGAPrint(text); }

uint32_t SyscallDispatch(SyscallRegs *regs) {
    switch (regs->eax) {
        case SYSCALL_EXIT:
            SysExit(regs->ebx);
            return 0;

        case SYSCALL_EXEC:
            return SysExec((const char *)regs->ebx);

        case SYSCALL_WRITE:
            SysWrite((const char *)regs->ebx);
            return 0;

        default:
            printf("Unknown syscall %u\n", regs->eax);
            return (uint32_t)-1;
    }
}
