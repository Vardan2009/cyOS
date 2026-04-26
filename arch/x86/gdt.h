#ifndef CY_GDT_H
#define CY_GDT_H

#include <stdint.h>

typedef struct {
    uint16_t limit;
    uint16_t baseL;
    uint8_t baseM;
    uint8_t access;
    uint8_t flags;
    uint8_t baseH;
} __attribute__((packed)) GDTEntry;

typedef struct {
    uint16_t limit;
    unsigned int base;
} __attribute__((packed)) GDTPtr;

typedef struct {
    uint32_t prevTSS;

    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;

    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;

    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;

    uint32_t ldt;
    uint32_t trap;
    uint32_t iomapBase;
} __attribute__((packed)) TSSEntry;

void GDTInit();
void GDTSetGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access,
                uint8_t gr);

void TSSWrite(uint32_t num, uint16_t ss0, uint32_t esp0);

void TSSSetKernelStack(uint32_t esp0);

#endif  // CY_GDT_H
