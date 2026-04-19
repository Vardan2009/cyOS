#ifndef CY_PIT_H
#define CY_PIT_H

#include "intregs.h"

void PITInit();
void PITOnIRQ0(IntRegs *regs);

#endif  // CY_PIT_H
