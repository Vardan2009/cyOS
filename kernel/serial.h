#ifndef CY_SERIAL_H
#define CY_SERIAL_H

#include <stdint.h>

#define COM1 0x3f8
#define COM2 0x2f8
#define COM3 0x3e8
#define COM4 0x2e8
#define COM5 0x2e8
#define COM6 0x4f8
#define COM7 0x5e8
#define COM8 0x4e8

#define DBGPORT COM1

void SerialInit(uint16_t port);

void SerialWrite(uint16_t port, char c);
void SerialWriteStr(uint16_t port, const char *str);

#endif  // CY_SERIAL_H
