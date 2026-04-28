#ifndef CY_PROCESS_H
#define CY_PROCESS_H

#include <stdint.h>

#include "fd.h"

#define USER_STACK_TOP 0xBFFFF000
#define USER_STACK_SIZE 0x5000

#define PROCESS_MAX_ARGS 32
#define PROCESS_MAX_ENV 32

typedef struct _Process {
    uint32_t *pageDir;
    uint32_t entry;
    uint32_t stackTop;
    uint32_t kernelStack;
    int exitCode;

    uint32_t returnEbp;
    struct _Process *parent;

    FileDescriptor fds[FD_MAX];

    char *argv[PROCESS_MAX_ARGS];
    char *envp[PROCESS_MAX_ENV];
    int argc;
    int envc;

    uint32_t heapBreak;
} Process;

extern Process *currentProcess;

uint32_t *ProcessCreateAddressSpace();
void ProcessMapPage(uint32_t *pdPhys, uint32_t vaddr, uint32_t paddr,
                    uint32_t flags);

void ProcessExecute(Process *proc);
void ProcessFreePages(Process *proc);

#endif  // CY_PROCESS_H
