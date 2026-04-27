#include <stdio.h>

#include "syscall.h"

void _start() {
    char buf[512];

    while (1) {
        puts("\ncysh $ ");
        if (gets(buf, sizeof(buf))) exec(buf);
    }
}
