#include <stdint.h>
#include <stdio.h>

int main(int argc, char **argv, char **envp) {
    printf("Hello, world!\n");

    printf("argv: \n");

    for (int i = 0; i < argc; ++i) printf(" - '%s'\n", argv[i]);

    printf("envp: \n");

    for (int i = 0; envp[i] != NULL; i++) printf(" - %s\n", envp[i]);

    return 0;
}
