#include <string.h>

void *memset(void *dest, char val, uint32_t n) {
    unsigned char *p = (unsigned char *)dest;
    while (n--) *p++ = (unsigned char)val;
    return dest;
}
