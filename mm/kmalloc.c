#include "kmalloc.h"

#include <stdio.h>

#include "math.h"
#include "memory.h"

static uint32_t heapStart, heapSize, threshold;

static uint8_t kmallocInitialized = 0;

void ChangeHeapSize(int newSize) {
    int oldPageTop = CEIL_DIV(heapSize, 0x1000);
    int newPageTop = CEIL_DIV(newSize, 0x1000);

    if (newPageTop > oldPageTop) {
        int diff = newPageTop - oldPageTop;

        for (int i = 0; i < diff; ++i) {
            uint32_t phys = PMMAllocPageFrame();
            MemMapPage(KERNEL_MALLOC + oldPageTop * 0x1000 + i * 0x1000, phys,
                       PAGE_FLAG_WRITE);
        }
    }

    heapSize = newSize;
}

void KMallocInit(uint32_t initialHeapSize) {
    heapStart = KERNEL_MALLOC;
    heapSize = 0;
    threshold = 0;
    kmallocInitialized = 1;

    ChangeHeapSize(initialHeapSize);
    *((uint32_t *)heapStart) = 0;

    printf("Heap start: %x\nHeap size: %x\n", heapStart, heapSize);
}
