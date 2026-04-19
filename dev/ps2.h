#ifndef CY_PS2_H
#define CY_PS2_H

#include <intregs.h>

void PS2KBInit();
void PS2KBHandler(IntRegs *regs);

char PS2KBGet();
uint8_t PS2KBEmpty();

#endif  // CY_PS2_H
