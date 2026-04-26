#include "elf.h"

#include <stdio.h>
#include <string.h>

#include "fatfs/ff.h"
#include "kmalloc.h"
#include "memory.h"
#include "process.h"

static void AllocUserRange(uint32_t *pdPhys, uint32_t vaddr, uint32_t size,
                           uint32_t flags) {
    uint32_t pageStart = vaddr & ~0xFFF;
    uint32_t pageEnd = (vaddr + size + 0xFFF) & ~0xFFF;

    for (uint32_t p = pageStart; p < pageEnd; p += 0x1000) {
        uint32_t frame = PMMAllocPageFrame();
        ProcessMapPage(pdPhys, p, frame,
                       flags | PAGE_FLAG_PRESENT | PAGE_FLAG_USER);
    }
}

#include "fd.h"

Process *ELFLoad(const char *path) {
    FIL file;
    UINT bytesRead;

    if (f_open(&file, path, FA_READ) != FR_OK) {
        printf("ELF: can't open %s\n", path);
        return NULL;
    }

    ELFHeader hdr;
    f_read(&file, &hdr, sizeof(hdr), &bytesRead);

    /* printf(
         "=== ELF Header ===\n - Magic: %x\n - Class: %x\n - Type: %x\n - "
         "Architecture: %x\n",
         hdr.magic, hdr.class, hdr.type, hdr.arch);
 */

    if (hdr.magic != ELF_MAGIC || hdr.class != ELF_CLASS_32 ||
        hdr.type != ELF_TYPE_EXEC || hdr.arch != ELF_ARCH_X86) {
        printf("ELF: invalid header\n");
        f_close(&file);
        return NULL;
    }

    uint32_t *pdPhys = ProcessCreateAddressSpace();

    for (int i = 0; i < hdr.ph_count; i++) {
        ELFProgramHeader ph;

        f_lseek(&file, hdr.ph_offset + i * hdr.ph_entry_size);
        f_read(&file, &ph, sizeof(ph), &bytesRead);

        if (ph.type != PT_LOAD) continue;
        if (ph.memSize == 0) continue;

        uint8_t *kbuf = kmalloc(ph.fileSize);
        if (!kbuf) {
            f_close(&file);
            return NULL;
        }

        f_lseek(&file, ph.offset);
        f_read(&file, kbuf, ph.fileSize, &bytesRead);

        if (bytesRead != ph.fileSize) {
            printf("ELF: short read: got %u expected %u\n", bytesRead,
                   ph.fileSize);
            kfree(kbuf);
            f_close(&file);
            return NULL;
        }

        /* printf("ELF: loading segment vaddr=0x%x size=0x%x flags=%x\n",
           ph.vaddr, ph.memSize, ph.flags); */

        uint32_t flags = PAGE_FLAG_OWNER;
        if (ph.flags & PF_W) flags |= PAGE_FLAG_WRITE;

        AllocUserRange(pdPhys, ph.vaddr, ph.memSize, flags);

        uint32_t old_cr3;
        asm volatile("mov %%cr3, %0" : "=r"(old_cr3));
        asm volatile("mov %0, %%cr3" ::"r"(pdPhys));

        memcpy((void *)ph.vaddr, kbuf, ph.fileSize);

        if (ph.memSize > ph.fileSize)
            memset((void *)(ph.vaddr + ph.fileSize), 0,
                   ph.memSize - ph.fileSize);

        asm volatile("mov %0, %%cr3" ::"r"(old_cr3));
        kfree(kbuf);
    }

    f_close(&file);

    AllocUserRange(pdPhys, USER_STACK_TOP - USER_STACK_SIZE, USER_STACK_SIZE,
                   PAGE_FLAG_WRITE | PAGE_FLAG_OWNER);

    Process *proc = kmalloc(sizeof(Process));
    proc->pageDir = pdPhys;
    proc->entry = hdr.entry;
    proc->stackTop = USER_STACK_TOP;

    if (currentProcess)
        FDInherit(proc, currentProcess);
    else
        FDInitProcess(proc);

    return proc;
}
