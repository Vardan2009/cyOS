#include <stdint.h>

char **environ = NULL;

void __libc_init(int argc, char **argv, char **envp) {
    environ = envp;
    (void)argc;
    (void)argv;
}
