#include <stdio.h>

int main(int argc, char **argv) {
    puts("Hello, world!\n");

    puts("argv: \n");

    for (int i = 0; i < argc; ++i) {
        puts(" - ");
        puts(argv[i]);
        puts("\n");
    }

    return 0;
}
