#include <string.h>

void *memset(void *dest, char val, uint32_t n) {
    unsigned char *p = (unsigned char *)dest;
    while (n--) *p++ = (unsigned char)val;
    return dest;
}

void *memcpy(void *dest, const void *src, uint32_t n) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    while (n--) *d++ = *s++;

    return dest;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}
