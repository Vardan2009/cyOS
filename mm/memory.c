#include "memory.h"

#include <string.h>

#include "math.h"

static uint32_t pageFrameMin, pageFrameMax, totalAlloc;

#define NUM_PAGES_DIRS 256
#define NUM_PAGE_FRAMES (0x100000000 / 0x1000)

uint8_t physMemBitmap[NUM_PAGE_FRAMES / 8];

static uint32_t pageDirs[NUM_PAGES_DIRS][1024] __attribute__((aligned(4096)));
static uint8_t pageDirUsed[NUM_PAGES_DIRS];

int memNumVPages;

void PMMInit(uint32_t memLow, uint32_t memHigh) {
    pageFrameMin = CEIL_DIV(memLow, 0x1000);
    pageFrameMax = memHigh / 0x1000;
    totalAlloc = 0;

    memset(physMemBitmap, 0, sizeof(physMemBitmap));
}

uint32_t PMMAllocPageFrame() {
    uint32_t start = pageFrameMin / 8 + ((pageFrameMin & 7) != 0 ? 1 : 0);
    uint32_t end = pageFrameMax / 8 - ((pageFrameMax & 7) != 0 ? 1 : 0);

    for (uint32_t b = start; b < end; ++b) {
        uint8_t byte = physMemBitmap[b];
        if (byte == 0xFF) continue;

        for (uint32_t i = 0; i < 8; ++i) {
            uint8_t used = byte >> i & 1;

            if (!used) {
                byte ^= (-1 ^ byte) & (1 << i);
                physMemBitmap[b] = byte;
                ++totalAlloc;

                uint32_t addr = (b * 8 + i) * 0x1000;
                return addr;
            }
        }
    }

    return 0;
}

uint32_t *MemGetCurrentPageDir() {
    uint32_t pd;
    asm volatile("MOV %%CR3, %0" : "=r"(pd));
    pd += KERNEL_START;

    return (uint32_t *)pd;
}

void MemChangePageDir(uint32_t *pd) {
    pd = (uint32_t *)(((uint32_t)pd) - KERNEL_START);
    asm volatile("MOV %0, %%EAX \n MOV %%EAX, %%CR3" ::"m"(pd));
}

void SyncPageDirs() {
    for (int i = 0; i < NUM_PAGES_DIRS; ++i) {
        if (!pageDirUsed[i]) continue;

        uint32_t *pageDir = pageDirs[i];

        for (int i = 768; i < 1023; ++i)
            pageDir[i] = initialPageDir[i] & ~PAGE_FLAG_OWNER;
    }
}

void MemMapPage(uint32_t vaddr, uint32_t paddr, uint32_t flags) {
    uint32_t *prevPageDir = 0;

    if (vaddr >= KERNEL_START) {
        prevPageDir = MemGetCurrentPageDir();
        if (prevPageDir != initialPageDir) MemChangePageDir(initialPageDir);
    }

    uint32_t pdIndex = vaddr >> 22;
    uint32_t ptIndex = vaddr >> 12 & 0x3FF;

    uint32_t *pageDir = REC_PAGEDIR;
    uint32_t *pt = REC_PAGETABLE(pdIndex);

    if (!(pageDir[pdIndex] & PAGE_FLAG_PRESENT)) {
        uint32_t ptPaddr = PMMAllocPageFrame();

        pageDir[pdIndex] = ptPaddr | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE |
                           PAGE_FLAG_OWNER | flags;

        Invalidate(vaddr);

        for (uint32_t i = 0; i < 1024; ++i) pt[i] = 0;
    }

    pt[ptIndex] = paddr | PAGE_FLAG_PRESENT | flags;
    ++memNumVPages;

    Invalidate(vaddr);

    if (prevPageDir != 0) {
        SyncPageDirs();
        if (prevPageDir != initialPageDir) MemChangePageDir(prevPageDir);
    }
}

void Invalidate(uint32_t vaddr) {
    asm volatile("invlpg (%0)" ::"r"(vaddr) : "memory");
}

void MemInit(uint32_t memHigh, uint32_t physAllocStart) {
    memNumVPages = 0;

    initialPageDir[0] = 0;
    Invalidate(0);

    initialPageDir[1023] = ((uint32_t)initialPageDir - KERNEL_START) |
                           PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;

    Invalidate(0xFFFFF000);

    PMMInit(physAllocStart, memHigh);

    memset(pageDirs, 0, 0x1000 * NUM_PAGES_DIRS);
    memset(pageDirUsed, 0, NUM_PAGES_DIRS);
}
