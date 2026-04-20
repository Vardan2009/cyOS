#ifndef CY_KMALLOC_H
#define CY_KMALLOC_H

#include <stdint.h>

void KMallocInit(uint32_t initialHeapSize);
void ChangeHeapSize(int newSize);

#endif  // CY_KMALLOC_H
