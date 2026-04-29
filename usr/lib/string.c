#include "string.h"

size_t strlen(const char *s) {
    size_t n = 0;
    while (s[n]) n++;
    return n;
}

char *strcpy(char *dst, const char *src) {
    char *d = dst;
    while ((*d++ = *src++));
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i]; i++) dst[i] = src[i];
    for (; i < n; i++) dst[i] = '\0';
    return dst;
}

int strcmp(const char *a, const char *b) {
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char *a, const char *b, size_t n) {
    while (n-- && *a && *a == *b) {
        a++;
        b++;
    }
    return n == (size_t)-1 ? 0 : (unsigned char)*a - (unsigned char)*b;
}

char *strchr(const char *s, int c) {
    while (*s) {
        if (*s == c) return (char *)s;
        s++;
    }
    return c == '\0' ? (char *)s : NULL;
}

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;
    while (n--) *d++ = *s++;
    return dst;
}

void *memset(void *dst, int c, size_t n) {
    uint8_t *d = dst;
    while (n--) *d++ = (uint8_t)c;
    return dst;
}

int memcmp(const void *a, const void *b, size_t n) {
    const uint8_t *x = a, *y = b;
    while (n--) {
        if (*x != *y) return *x - *y;
        x++;
        y++;
    }
    return 0;
}

size_t strcspn(const char *s, const char *reject) {
    const char *p = s;
    while (*p != '\0') {
        const char *r = reject;
        while (*r != '\0') {
            if (*p == *r) return (size_t)(p - s);
            r++;
        }
        p++;
    }
    return (size_t)(p - s);
}

char *strtok(char *str, const char *delim) {
    static char *lastToken = NULL;

    if (str != NULL) lastToken = str;

    if (lastToken == NULL || *lastToken == '\0') return NULL;

    char *tokenStart = lastToken;
    while (*tokenStart && strchr(delim, *tokenStart)) ++tokenStart;

    if (*tokenStart == '\0') {
        lastToken = NULL;
        return NULL;
    }

    char *tokenEnd = tokenStart;
    while (*tokenEnd && !strchr(delim, *tokenEnd)) ++tokenEnd;

    if (*tokenEnd != '\0') {
        *tokenEnd = '\0';
        lastToken = tokenEnd + 1;
    } else
        lastToken = NULL;

    return tokenStart;
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

#include "stdlib.h"

char *strdup(const char *s) {
    size_t len = strlen(s) + 1;
    void *newStr = malloc(len);
    if (newStr == NULL) return NULL;
    return (char *)memcpy(newStr, s, len);
}
