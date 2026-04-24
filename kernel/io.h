#ifndef CY_IO_H
#define CY_IO_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

static inline char inb(uint16_t port) {
    char r;
    asm volatile("inb %1, %0" : "=a"(r) : "dN"(port));
    return r;
}

static inline void outw(uint16_t port, uint16_t value) {
    asm volatile("outw %1, %0" : : "dN"(port), "a"(value));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t r;
    asm volatile("inw %1, %0" : "=a"(r) : "dN"(port));
    return r;
}

static inline void outl(uint16_t port, uint32_t value) {
    asm volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t value;
    asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

#endif  // CY_IO_H
