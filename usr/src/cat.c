#include <stdint.h>
#include <stdio.h>

#include "syscall.h"

#define BUF_SIZE 4096

void CatFD(int fd) {
    char buf[BUF_SIZE];
    ssize_t n;

    while ((n = read(fd, buf, BUF_SIZE)) > 0) {
        if (write(FD_STDOUT, buf, n) != n) {
            fprintf(FD_STDERR, "cat: write error\n");
            return;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(FD_STDERR, "usage: %s <PATH>\n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    CatFD(fd);
    close(fd);
}
