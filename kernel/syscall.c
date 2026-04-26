#include "syscall.h"

#include <stdio.h>

#include "idt.h"
#include "kmalloc.h"
#include "memory.h"
#include "process.h"

extern void SyscallStub();

void SyscallInit() { IDTSetGate(0x80, (uint32_t)SyscallStub, 0x08, 0xEF); }

static void SysExit(uint32_t code) {
    Process *proc = currentProcess;
    if (!proc) return;

    printf("[kernel] process exited with code %u\n", code);

    ProcessFreePages(proc);

    asm volatile("mov %0, %%cr3" ::"r"((uint32_t)initialPageDir -
                                       (uint32_t)KERNEL_START));

    asm volatile(
        "movw $0x10, %%ax \n"
        "movw %%ax, %%ds  \n"
        "movw %%ax, %%es  \n"
        "movw %%ax, %%fs  \n"
        "movw %%ax, %%gs  \n" ::
            : "eax");

    kfree((void *)(proc->kernelStack - 0x2000));
    kfree(proc);
    currentProcess = NULL;

    extern uint32_t returnEbp;
    asm volatile(
        "mov %0, %%ebp  \n"
        "leave          \n"
        "ret            \n" ::"r"(returnEbp));
}

uint32_t SyscallDispatch(SyscallRegs *regs) {
    printf("Syscall hit with EAX: %d\n", regs->eax);

    switch (regs->eax) {
        case SYSCALL_EXIT:
            SysExit(regs->ebx);
            return 0;

        case SYSCALL_EXEC:
        default:
            printf("Unknown syscall %u\n", regs->eax);
            return (uint32_t)-1;
    }
}
