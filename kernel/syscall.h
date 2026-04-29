#pragma once
#include <stdint.h>

#define SYSCALL_EXIT 1
#define SYSCALL_EXEC 2

#define SYSCALL_READ 3
#define SYSCALL_WRITE 4
#define SYSCALL_OPEN 5
#define SYSCALL_OPENDIR 6
#define SYSCALL_CLOSE 7
#define SYSCALL_SEEK 8
#define SYSCALL_STAT 9

#define SYSCALL_GETENV 10
#define SYSCALL_SETENV 11
#define SYSCALL_UNSETENV 12

#define SYSCALL_SBRK 13

#define SYSCALL_GETCWD 14
#define SYSCALL_CHDIR 15

typedef struct {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
} SyscallRegs;

typedef struct {
    uint32_t size;
    uint8_t isDir;
    uint8_t exists;
    char name[256];
} StatInfo;

void SyscallInit();
