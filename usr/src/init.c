#include <stdio.h>

#include "syscall.h"

void _start() {
    puts("Welcome to cyOS!\n\n");

    exec("1:/bin/shell");

    exit(0);
}
