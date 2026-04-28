#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv, char **envp) {
    printf("=== Hello, world! ===\n");

    printf("argv: \n");

    for (int i = 0; i < argc; ++i) printf(" - '%s'\n", argv[i]);

    printf("envp: \n");

    for (int i = 0; envp[i] != NULL; i++) printf(" - %s\n", envp[i]);

    printf("Heap test: \n");

    char *test = malloc(100);

    strcpy(test, "This is from the heap!");

    realloc(test, 512);

    printf(" - \"%s\"\n", test);

    free(test);

    return 0;
}
