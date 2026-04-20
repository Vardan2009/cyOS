#ifndef CY_STRING_H
#define CY_STRING_H

#include <stdint.h>

void *memset(void *dest, char val, uint32_t count);
void *memcpy(void *dest, const void *src, uint32_t n);

#endif  // CY_STRING_H
