#include "serial.h"

#include <io.h>

void SerialInit(uint16_t port) {
    outb(port + 1, 0x00);

    outb(port + 3, 0x80);

    outb(port + 0, 0x03);
    outb(port + 1, 0x00);

    outb(port + 3, 0x03);

    outb(port + 2, 0xC7);
    outb(port + 4, 0x0B);
}

uint8_t SerialTransmitEmpty(uint16_t port) { return inb(port + 5) & 0x20; }

void SerialWrite(uint16_t port, char c) {
    while (SerialTransmitEmpty(port) == 0);
    outb(port, c);
}

void SerialWriteStr(uint16_t port, const char *str) {
    while (*str) SerialWrite(port, *str++);
}
