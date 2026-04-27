#include <stdio.h>

#include "syscall.h"

int main(int argc, char **argv) {
    printf("Welcome to cyOS!\n\n");

    const char *shargv[] = {"1:/bin/shell", NULL};

    exec("1:/bin/shell", (char **)shargv);

    return 0;
}
