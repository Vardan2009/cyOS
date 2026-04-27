#include <stdint.h>
#include <stdio.h>

#include "syscall.h"

int main(int argc, char **argv) {
    char buf[512];

    const char *pargv[] = {"a", "b", "c", NULL};

    while (1) {
        printf("\ncysh $ ");
        scanf("%s", buf);
        exec(buf, (char **)pargv);
    }

    return 0;
}
