#include "gdt.h"

#include <stdint.h>

extern void GDTFlush(addr_t);

GDTEntry gdtEntries[5];
GDTPtr gdtPtr;

void GDTInit() {
    gdtPtr.limit = 5 * sizeof(GDTEntry) - 1;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-conversion"
    gdtPtr.base = (unsigned int)&gdtEntries;
#pragma GCC diagnostic pop

    GDTSetGate(0, 0, 0, 0, 0);                                 // null
    GDTSetGate(1, 0, 0xFFFFFFFF, 0x9A /* 1001 1010 */, 0xCF);  // kernel code
    GDTSetGate(2, 0, 0xFFFFFFFF, 0x92 /* 1001 0010 */, 0xCF);  // kernel data
    GDTSetGate(3, 0, 0xFFFFFFFF, 0xFA /* 1111 1010 */, 0xCF);  // user code
    GDTSetGate(4, 0, 0xFFFFFFFF, 0xF2 /* 1111 0010 */, 0xCF);  // user data

    GDTFlush((addr_t)&gdtPtr);
}

void GDTSetGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access,
                uint8_t gr) {
    gdtEntries[num].baseL = base & 0xFFFF;
    gdtEntries[num].baseM = (base >> 16) & 0xFF;
    gdtEntries[num].baseH = (base >> 24) & 0xFF;

    gdtEntries[num].limit = limit & 0xFFFF;
    gdtEntries[num].flags = (limit >> 16) & 0x0F;
    gdtEntries[num].flags |= gr & 0xF0;

    gdtEntries[num].access = access;
}
