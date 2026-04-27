#ifndef CYLIBC_SYSCALL_H
#define CYLIBC_SYSCALL_H

#include <stdint.h>

static inline uint32_t syscall1(uint32_t num, uint32_t a1) {
    uint32_t ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(a1) : "memory");
    return ret;
}

static inline uint32_t syscall2(uint32_t num, uint32_t a1, uint32_t a2) {
    uint32_t ret;
    asm volatile("int $0x80"
                 : "=a"(ret)
                 : "a"(num), "b"(a1), "c"(a2)
                 : "memory");
    return ret;
}

static inline uint32_t syscall3(uint32_t num, uint32_t a1, uint32_t a2,
                                uint32_t a3) {
    uint32_t ret;
    asm volatile("int $0x80"
                 : "=a"(ret)
                 : "a"(num), "b"(a1), "c"(a2), "d"(a3)
                 : "memory");
    return ret;
}

#define SYSCALL_EXIT 1
#define SYSCALL_EXEC 2
#define SYSCALL_READ 3
#define SYSCALL_WRITE 4
#define SYSCALL_OPEN 5
#define SYSCALL_CLOSE 6
#define SYSCALL_SEEK 7

void exit(int code);

static inline int exec(const char *path, char **argv) {
    return (int)syscall2(SYSCALL_EXEC, (uint32_t)path, (uint32_t)argv);
}

static inline int open(const char *path, int flags) {
    return (int)syscall2(SYSCALL_OPEN, (uint32_t)path, (uint32_t)flags);
}

static inline int close(int fd) {
    return (int)syscall1(SYSCALL_CLOSE, (uint32_t)fd);
}

static inline int read(int fd, void *buf, uint32_t count) {
    return (int)syscall3(SYSCALL_READ, (uint32_t)fd, (uint32_t)buf, count);
}

static inline int write(int fd, const void *buf, uint32_t count) {
    return (int)syscall3(SYSCALL_WRITE, (uint32_t)fd, (uint32_t)buf, count);
}

#endif  // CYLIBC_SYSCALL_H
