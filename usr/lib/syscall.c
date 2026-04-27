#include "syscall.h"

void exit(int code) {
    syscall1(SYSCALL_EXIT, (uint32_t)code);
    for (;;);
}
