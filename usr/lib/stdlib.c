#include "stdlib.h"

#include <string.h>

#include "syscall.h"

extern char **environ;

char *getenv(const char *key) {
    if (environ) {
        int klen = strlen(key);
        for (int i = 0; environ[i]; i++) {
            if (strncmp(environ[i], key, klen) == 0 &&
                environ[i][klen] == '=') {
                return environ[i] + klen + 1;
            }
        }
    }
    return NULL;
}

int setenv(const char *key, const char *value, int overwrite) {
    int klen = strlen(key);
    int vlen = strlen(value);

    char *entry = malloc(klen + vlen + 2);
    if (!entry) return -1;

    memcpy(entry, key, klen);
    entry[klen] = '=';
    memcpy(entry + klen + 1, value, vlen + 1);

    for (int i = 0; environ && environ[i]; i++) {
        if (strncmp(environ[i], key, klen) == 0 && environ[i][klen] == '=') {
            if (!overwrite) {
                free(entry);
                return 0;
            }

            free(environ[i]);
            environ[i] = entry;

            syscall2(SYSCALL_SETENV, (uint32_t)key, (uint32_t)value);
            return 0;
        }
    }

    int count = 0;
    while (environ && environ[count]) count++;

    char **newenv = realloc(environ, sizeof(char *) * (count + 2));
    if (!newenv) {
        free(entry);
        return -1;
    }

    environ = newenv;
    environ[count] = entry;
    environ[count + 1] = NULL;

    syscall2(SYSCALL_SETENV, (uint32_t)key, (uint32_t)value);
    return 0;
}

int unsetenv(const char *key) {
    int klen = strlen(key);

    for (int i = 0; environ && environ[i]; i++) {
        if (strncmp(environ[i], key, klen) == 0 && environ[i][klen] == '=') {
            free(environ[i]);

            for (int j = i; environ[j]; j++) {
                environ[j] = environ[j + 1];
            }

            syscall1(SYSCALL_UNSETENV, (uint32_t)key);
            return 0;
        }
    }

    return 0;
}

int atoi(const char *s) {
    int n = 0, neg = 0;
    while (*s == ' ') s++;
    if (*s == '-') {
        neg = 1;
        s++;
    } else if (*s == '+')
        s++;
    while (*s >= '0' && *s <= '9') n = n * 10 + (*s++ - '0');
    return neg ? -n : n;
}

typedef struct Block {
    size_t size;
    int free;
    struct Block *next;
    struct Block *prev;
} Block;

static Block *heapHead = NULL;

void *malloc(size_t size) {
    size = (size + 3) & ~3;

    if (!heapHead) {
        heapHead = sbrk(sizeof(Block) + size);
        heapHead->size = size;
        heapHead->free = 0;
        heapHead->next = NULL;
        heapHead->prev = NULL;
        return (void *)(heapHead + 1);
    }

    Block *cur = heapHead;
    while (cur) {
        if (cur->free && cur->size >= size) {
            if (cur->size >= size + sizeof(Block) + 32) {
                Block *nb = (Block *)((uint8_t *)(cur + 1) + size);
                nb->size = cur->size - size - sizeof(Block);
                nb->free = 1;
                nb->next = cur->next;
                nb->prev = cur;
                if (cur->next) cur->next->prev = nb;
                cur->next = nb;
                cur->size = size;
            }
            cur->free = 0;
            return (void *)(cur + 1);
        }
        if (!cur->next) {
            Block *nb = sbrk(sizeof(Block) + size);
            nb->size = size;
            nb->free = 0;
            nb->next = NULL;
            nb->prev = cur;
            cur->next = nb;
            return (void *)(nb + 1);
        }
        cur = cur->next;
    }
    return NULL;
}

void free(void *ptr) {
    if (!ptr) return;
    Block *b = (Block *)ptr - 1;
    b->free = 1;
    if (b->next && b->next->free) {
        b->size += sizeof(Block) + b->next->size;
        b->next = b->next->next;
        if (b->next) b->next->prev = b;
    }
    if (b->prev && b->prev->free) {
        b->prev->size += sizeof(Block) + b->size;
        b->prev->next = b->next;
        if (b->next) b->next->prev = b->prev;
    }
}

void *realloc(void *ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (!size) {
        free(ptr);
        return NULL;
    }
    Block *b = (Block *)ptr - 1;
    if (b->size >= size) return ptr;
    void *new = malloc(size);
    if (!new) return NULL;
    memcpy(new, ptr, b->size);
    free(ptr);
    return new;
}

static void QsortSwap(uint8_t *a, uint8_t *b, size_t size) {
    while (size--) {
        uint8_t tmp = *a;
        *a++ = *b;
        *b++ = tmp;
    }
}

void qsort(void *base, size_t count, size_t size,
           int (*cmp)(const void *, const void *)) {
    if (count < 2) return;
    uint8_t *arr = (uint8_t *)base;

    // insertion sort ( yeah no quicksort :) )
    for (size_t i = 1; i < count; i++) {
        size_t j = i;
        while (j > 0 && cmp(arr + (j - 1) * size, arr + j * size) > 0) {
            QsortSwap(arr + (j - 1) * size, arr + j * size, size);
            --j;
        }
    }
}
