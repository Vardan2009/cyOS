#include <stdio.h>

#include "syscall.h"

int main(int argc, char **argv) {
    char buf[512];

    puts("shell argv: \n");

    for (int i = 0; i < argc; ++i) {
        puts(" - ");
        puts(argv[i]);
        puts("\n");
    }

    const char *pargv[] = {"a", "b", "c", NULL};

    while (1) {
        puts("\ncysh $ ");
        if (gets(buf, sizeof(buf))) exec(buf, (char **)pargv);
    }

    return 0;
}
