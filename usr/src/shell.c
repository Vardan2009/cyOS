#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syscall.h"

#define MAX_ARGS 16

int main(int argc, char **argv) {
    char buf[512];
    char *pargv[MAX_ARGS];

    while (1) {
        printf("cysh $ ");

        if (gets(buf, sizeof(buf)) == NULL) break;

        buf[strcspn(buf, "\n")] = '\0';

        int i = 0;
        char *token = strtok(buf, " ");

        if (token == NULL) continue;

        while (token != NULL && i < MAX_ARGS - 1) {
            if (token[0] != '$')
                pargv[i++] = token;
            else
                pargv[i++] = getenv(token + 1);
            token = strtok(NULL, " ");
        }
        pargv[i] = NULL;

        exec(pargv[0], (char **)pargv);
    }

    return 0;
}
