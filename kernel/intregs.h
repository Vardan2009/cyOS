#ifndef CY_INTREGS_H
#define CY_INTREGS_H

#include <stdint.h>

typedef struct {
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t intno, errno;
    uint32_t eip, csm, eflags, useresp, ss;
} __attribute__((packed)) IntRegs;

#endif  // CY_INTREGS_H
