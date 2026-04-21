#ifndef CY_IO_H
#define CY_IO_H

#include <stdint.h>

void outb(uint16_t port, uint8_t value);
char inb(uint16_t port);

uint16_t inw(uint16_t port);

#endif  // CY_IO_H
