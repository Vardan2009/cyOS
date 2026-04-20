#include "inbuffer.h"

uint8_t shift, caps, ctrl;

#define INPUT_BUFFER_SIZE 128
char inputBuffer[INPUT_BUFFER_SIZE];
int bufferStart = 0;
int bufferEnd = 0;

void InBufferPutC(char c) {
    inputBuffer[bufferEnd] = c;
    bufferEnd = (bufferEnd + 1) % INPUT_BUFFER_SIZE;
}

char InBufferGetC() {
    if (bufferStart == bufferEnd) return 0;
    char c = inputBuffer[bufferStart];
    bufferStart = (bufferStart + 1) % INPUT_BUFFER_SIZE;
    return c;
}

uint8_t InBufferEmpty() { return bufferStart == bufferEnd; }
