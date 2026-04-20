#ifndef CY_INBUFFER_H
#define CY_INBUFFER_H

#include <stdint.h>

extern uint8_t shift, caps, ctrl;

void InBufferPutC(char c);
char InBufferGetC();

uint8_t InBufferEmpty();

#endif  // CY_INBUFFER_H
