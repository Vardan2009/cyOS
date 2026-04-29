#include <stdlib.h>
#include <string.h>

char **environ = NULL;

void __libc_init(int argc, char **argv, char **envp) {
    (void)argc;
    (void)argv;

    size_t count = 0;
    while (envp[count] != NULL) count++;

    environ = malloc((count + 1) * sizeof(char *));
    if (!environ) return;

    for (size_t i = 0; i < count; i++) {
        environ[i] = strdup(envp[i]);
    }

    environ[count] = NULL;
}
