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

int strncmp(const char *s1, const char *s2, uint32_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    return n == 0 ? 0 : *(unsigned char *)s1 - *(unsigned char *)s2;
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

char *strcpy(char *dest, const char *src) {
    char *ptr = dest;
    while ((*ptr++ = *src++)) {
    }
    return dest;
}

char *strncpy(char *dest, const char *src, uint32_t n) {
    char *ptr = dest;
    while (n > 0 && *src) {
        *ptr++ = *src++;
        --n;
    }
    while (n > 0) {
        *ptr++ = '\0';
        --n;
    }
    return dest;
}

char *strcat(char *dest, const char *src) {
    char *ptr = dest;
    while (*ptr) {
        ptr++;
    }
    while ((*ptr++ = *src++)) {
    }
    return dest;
}

char *strsep(char **str, const char *delim) {
    if (!*str) return NULL;
    char *start = *str;
    char *p = start;
    while (*p) {
        const char *d = delim;
        while (*d) {
            if (*p == *d) {
                *p = '\0';
                *str = p + 1;
                return start;
            }
            ++d;
        }
        ++p;
    }
    *str = NULL;
    return start;
}

char *strncat(char *dest, const char *src, uint32_t n) {
    char *ptr = dest;
    while (*ptr) ptr++;
    while (n-- && (*ptr++ = *src++));
    *ptr = '\0';
    return dest;
}
