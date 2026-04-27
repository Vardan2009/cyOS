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

int memcmp(const void *ptr1, const void *ptr2, uint32_t num) {
    const unsigned char *p1 = (const unsigned char *)ptr1;
    const unsigned char *p2 = (const unsigned char *)ptr2;

    for (uint32_t i = 0; i < num; i++)
        if (p1[i] != p2[i]) return (p1[i] < p2[i]) ? -1 : 1;

    return 0;
}

char *strchr(const char *str, int c) {
    while (*str) {
        if (*str == c) return (char *)str;
        str++;
    }
    if (c == '\0') return (char *)str;
    return NULL;
}

uint32_t strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return (uint32_t)(p - s);
}
