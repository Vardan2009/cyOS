#include "process.h"

#include <string.h>

#include "memory.h"

Process *currentProcess = NULL;

uint32_t *ProcessCreateAddressSpace() {
    uint32_t *pdPhys = (uint32_t *)PMMAllocPageFrame();

    uint32_t pdVirt = 0xE0000000;
    MemMapPage(pdVirt, (uint32_t)pdPhys,
               PAGE_FLAG_WRITE | PAGE_FLAG_PRESENT | PAGE_FLAG_USER);

    uint32_t *pd = (uint32_t *)pdVirt;
    memset(pd, 0, 0x1000);

    for (int i = 768; i < 1024; i++) pd[i] = initialPageDir[i];

    pd[1023] =
        (uint32_t)pdPhys | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE | PAGE_FLAG_USER;

    return pdPhys;
}

void ProcessMapPage(uint32_t *pdPhys, uint32_t vaddr, uint32_t paddr,
                    uint32_t flags) {
    uint32_t oldcr3;
    asm volatile("mov %%cr3, %0" : "=r"(oldcr3));
    asm volatile("mov %0, %%cr3" ::"r"(pdPhys));

    MemMapPage(vaddr, paddr, flags);

    asm volatile("mov %0, %%cr3" ::"r"(oldcr3));
}

#include "gdt.h"
#include "kmalloc.h"

#define USER_CODE_SEG 0x1B
#define USER_DATA_SEG 0x23

uint32_t returnEbp = 0;

void ProcessExecute(Process *proc) {
    uint32_t kstack = (uint32_t)kmalloc(0x2000) + 0x2000;
    proc->kernelStack = kstack;
    TSSSetKernelStack(kstack);
    currentProcess = proc;

    asm volatile("mov %%ebp, %0" : "=m"(returnEbp));

    asm volatile("mov %0, %%cr3" ::"r"(proc->pageDir));

    asm volatile(
        "movw %w0, %%ax     \n"
        "movw %%ax, %%ds    \n"
        "movw %%ax, %%es    \n"
        "movw %%ax, %%fs    \n"
        "movw %%ax, %%gs    \n"
        "pushl %0           \n"
        "pushl %1           \n"
        "pushfl             \n"
        "popl %%eax         \n"
        "orl $0x200, %%eax  \n"
        "pushl %%eax        \n"
        "pushl %2           \n"
        "pushl %3           \n"
        "iretl              \n" ::"r"((uint32_t)USER_DATA_SEG),
        "r"(proc->stackTop), "r"((uint32_t)USER_CODE_SEG), "r"(proc->entry)
        : "eax");
}

void ProcessFreePages(Process *proc) {
    uint32_t old_cr3;
    asm volatile("mov %%cr3, %0" : "=r"(old_cr3));
    asm volatile("mov %0, %%cr3" ::"r"(proc->pageDir));

    for (int i = 0; i < 768; i++) {
        if (!(REC_PAGEDIR[i] & PAGE_FLAG_PRESENT)) continue;

        uint32_t *pt = REC_PAGETABLE(i);
        for (int j = 0; j < 1024; j++) {
            if ((pt[j] & PAGE_FLAG_PRESENT) && (pt[j] & PAGE_FLAG_OWNER)) {
                PMMFreePageFrame(pt[j] & ~0xFFF);
                pt[j] = 0;
            }
        }

        PMMFreePageFrame(REC_PAGEDIR[i] & ~0xFFF);
        REC_PAGEDIR[i] = 0;
    }

    asm volatile("mov %0, %%cr3" ::"r"(old_cr3));

    PMMFreePageFrame((uint32_t)proc->pageDir);
}
