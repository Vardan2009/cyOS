#include "io.h"

void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

char inb(uint16_t port) {
    char r;
    asm volatile("inb %1, %0" : "=a"(r) : "dN"(port));
    return r;
}
