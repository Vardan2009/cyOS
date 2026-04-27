#include <stdio.h>

int main(int argc, char **argv) {
    printf("Hello, world!\n");

    printf("argv: \n");

    for (int i = 0; i < argc; ++i) {
        printf(" - ");
        printf(argv[i]);
        printf("\n");
    }

    return 0;
}
