#ifndef CY_PROCESS_H
#define CY_PROCESS_H

#include <stdint.h>

// #define USER_STACK_TOP 0xBFFFFFFF
// #define USER_STACK_SIZE 0x4000

#define USER_STACK_TOP 0xBFFFF000
#define USER_STACK_SIZE 0x5000

typedef struct {
    uint32_t *pageDir;
    uint32_t entry;
    uint32_t stackTop;
} Process;

uint32_t *ProcessCreateAddressSpace();
void ProcessMapPage(uint32_t *pdPhys, uint32_t vaddr, uint32_t paddr,
                    uint32_t flags);

void ProcessExecute(Process *proc);

#endif  // CY_PROCESS_H
