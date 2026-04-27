#include <stdio.h>
#include <stdlib.h>

#include "syscall.h"

int main(int argc, char **argv) {
    printf("Welcome to cyOS!\n\n");

    setenv("SHELL", "1:/bin/shell", 1);

    const char *shargv[] = {"1:/bin/shell", NULL};

    exec("1:/bin/shell", (char **)shargv);

    return 0;
}
