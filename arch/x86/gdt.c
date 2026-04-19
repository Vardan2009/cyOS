#include "gdt.h"

#include <stdint.h>
#include <string.h>

extern void GDTFlush(addr_t);
extern void TSSFlush();

GDTEntry gdtEntries[6];
GDTPtr gdtPtr;
TSSEntry tssEntry;

void GDTInit() {
    gdtPtr.limit = 6 * sizeof(GDTEntry) - 1;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-conversion"
    gdtPtr.base = (unsigned int)&gdtEntries;
#pragma GCC diagnostic pop

    GDTSetGate(0, 0, 0, 0, 0);                                 // null
    GDTSetGate(1, 0, 0xFFFFFFFF, 0x9A /* 1001 1010 */, 0xCF);  // kernel code
    GDTSetGate(2, 0, 0xFFFFFFFF, 0x92 /* 1001 0010 */, 0xCF);  // kernel data
    GDTSetGate(3, 0, 0xFFFFFFFF, 0xFA /* 1111 1010 */, 0xCF);  // user code
    GDTSetGate(4, 0, 0xFFFFFFFF, 0xF2 /* 1111 0010 */, 0xCF);  // user data
    TSSWrite(5, 0x10, 0x0);  // task state segment

    GDTFlush((addr_t)&gdtPtr);
    TSSFlush();
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

void TSSWrite(uint32_t num, uint16_t ss0, uint32_t esp0) {
    uint32_t base = (uint32_t)&tssEntry;
    uint32_t limit = base + sizeof(tssEntry);

    GDTSetGate(num, base, limit, 0xE9 /* 1110 1001 */, 0x00);

    memset(&tssEntry, 0, sizeof(tssEntry));

    tssEntry.ss0 = ss0;
    tssEntry.esp0 = esp0;

    tssEntry.cs = 0x08 | 0x3;
    tssEntry.ss = tssEntry.ds = tssEntry.es = tssEntry.fs = tssEntry.gs =
        0x10 | 0x3;
}
