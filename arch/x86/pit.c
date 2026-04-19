#include "pit.h"

#include <io.h>
#include <stdint.h>

#include "idt.h"

uint64_t ticks;
const uint32_t freq = 100;

void PITInit() {
    ticks = 0;
    IRQInstallHandler(0, &PITOnIRQ0);

    uint32_t div = 1193180 / freq;

    outb(0x43, 0x36 /* 0011 0110 */);

    outb(0x40, (uint8_t)(div & 0xFF));
    outb(0x40, (uint8_t)((div >> 8) & 0xFF));
}

void PITOnIRQ0(IntRegs *regs) { ++ticks; }
