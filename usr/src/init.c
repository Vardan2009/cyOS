#include <stdio.h>
#include <stdlib.h>

#include "ansi.h"
#include "syscall.h"

int main(int argc, char **argv) {
    printf(ANSI_BOLD ANSI_FG_CYAN "Welcome to cyOS!\n\n" ANSI_RESET);

    setenv("SHELL", "1:/bin/cysh", 1);

    const char *shargv[] = {"1:/bin/cysh", NULL};

    exec("1:/bin/cysh", (char **)shargv);

    return 0;
}
