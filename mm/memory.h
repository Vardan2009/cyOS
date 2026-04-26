#ifndef CY_PMM_H
#define CY_PMM_H

#include <stdint.h>

#define KERNEL_START 0xC0000000
#define KERNEL_MALLOC 0xD0000000
#define REC_PAGEDIR ((uint32_t *)0xFFFFF000)
#define REC_PAGETABLE(I) ((uint32_t *)(0xFFC00000 + ((I) << 12)))

#define PAGE_FLAG_PRESENT (1 << 0)
#define PAGE_FLAG_WRITE (1 << 1)
#define PAGE_FLAG_USER (1 << 2)
#define PAGE_FLAG_OWNER (1 << 9)

extern uint32_t initialPageDir[1024];

void MemInit(uint32_t memHigh, uint32_t physAllocStart);

uint32_t PMMAllocPageFrame();

void Invalidate(uint32_t vaddr);

void MemMapPage(uint32_t vaddr, uint32_t paddr, uint32_t flags);

void PMMFreePageFrame(uint32_t addr);

#endif  // CY_PMM_H
