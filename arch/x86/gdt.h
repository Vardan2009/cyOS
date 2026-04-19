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

void GDTInit();
void GDTSetGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access,
                uint8_t gr);

#endif  // CY_GDT_H
