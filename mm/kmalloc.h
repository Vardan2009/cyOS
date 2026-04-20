#ifndef CY_KMALLOC_H
#define CY_KMALLOC_H

#include <stdint.h>

#define HEAP_MIN_SPLIT 32

typedef struct BlockHeader {
    // without header
    uint32_t size;
    uint8_t free;
    struct BlockHeader *next;
    struct BlockHeader *prev;
} BlockHeader;

void KMallocInit(uint32_t initialHeapSize);
void ChangeHeapSize(int newSize);

void *kmalloc(uint32_t size);
void *realloc(void *ptr, uint32_t newSize);
void kfree(void *ptr);

#endif  // CY_KMALLOC_H
