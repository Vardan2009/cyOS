#include "kmalloc.h"

#include <stdio.h>
#include <string.h>

#include "math.h"
#include "memory.h"

static uint32_t heapStart, heapSize, threshold;

static uint8_t kmallocInitialized = 0;

static BlockHeader *heapHead = NULL;

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

    heapHead = (BlockHeader *)heapStart;
    heapHead->size = initialHeapSize - sizeof(BlockHeader);
    heapHead->free = 1;
    heapHead->next = NULL;
    heapHead->prev = NULL;

    printf("Heap start: %x\nHeap size:  %d\n", heapStart, heapSize);
}

void *kmalloc(uint32_t size) {
    if (!kmallocInitialized || size == 0) return NULL;

    size = (size + 3) & ~3;

    BlockHeader *cur = heapHead;

    while (cur) {
        if (cur->free && cur->size >= size) {
            if (cur->size >= size + sizeof(BlockHeader) + HEAP_MIN_SPLIT) {
                BlockHeader *newBlock =
                    (BlockHeader *)((uint8_t *)(cur + 1) + size);
                newBlock->size = cur->size - size - sizeof(BlockHeader);
                newBlock->free = 1;
                newBlock->next = cur->next;
                newBlock->prev = cur;

                if (cur->next) cur->next->prev = newBlock;

                cur->next = newBlock;
                cur->size = size;
            }

            cur->free = 0;
            return (void *)(cur + 1);
        }

        if (!cur->next) {
            uint32_t expandBy = size + sizeof(BlockHeader);
            ChangeHeapSize(heapSize + expandBy);

            if (cur->free) {
                cur->size += expandBy;
                continue;
            }

            BlockHeader *newBlock =
                (BlockHeader *)((uint8_t *)(cur + 1) + cur->size);
            newBlock->size = expandBy - sizeof(BlockHeader);
            newBlock->free = 1;
            newBlock->next = NULL;
            newBlock->prev = cur;
            cur->next = newBlock;
        }

        cur = cur->next;
    }

    return NULL;
}

void kfree(void *ptr) {
    if (!ptr) return;

    BlockHeader *hdr = (BlockHeader *)ptr - 1;
    hdr->free = 1;

    if (hdr->next && hdr->next->free) {
        hdr->size += sizeof(BlockHeader) + hdr->next->size;
        hdr->next = hdr->next->next;
        if (hdr->next) hdr->next->prev = hdr;
    }

    if (hdr->prev && hdr->prev->free) {
        hdr->prev->size += sizeof(BlockHeader) + hdr->size;
        hdr->prev->next = hdr->next;
        if (hdr->next) hdr->next->prev = hdr->prev;
    }
}

void *krealloc(void *ptr, uint32_t newSize) {
    if (!ptr) return kmalloc(newSize);
    if (newSize == 0) {
        kfree(ptr);
        return NULL;
    }

    BlockHeader *hdr = (BlockHeader *)ptr - 1;
    if (hdr->size >= newSize) return ptr;

    void *newPtr = kmalloc(newSize);
    if (!newPtr) return NULL;

    memcpy(newPtr, ptr, hdr->size);
    kfree(ptr);
    return newPtr;
}
