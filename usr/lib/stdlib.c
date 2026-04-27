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
