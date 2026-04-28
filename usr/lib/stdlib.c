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
    if (!overwrite && getenv(key)) return 0;
    return (int)syscall2(SYSCALL_SETENV, (uint32_t)key, (uint32_t)value);
}

int unsetenv(const char *key) {
    return (int)syscall1(SYSCALL_UNSETENV, (uint32_t)key);
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
