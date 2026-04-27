#ifndef CYLIBC_STDLIB_H
#define CYLIBC_STDLIB_H

char *getenv(const char *key);
int setenv(const char *key, const char *value, int overwrite);
int unsetenv(const char *key);

int atoi(const char *s);

#endif  // CYLIBC_STDLIB_H
