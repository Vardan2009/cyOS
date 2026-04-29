#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syscall.h"

#define MAX_ARGS 32
#define PATH_MAX 256

static inline int FileExists(const char *path) {
    StatInfo info;
    return stat(path, &info) == 0 && info.exists;
}

static int PathLookup(const char *name, char *out) {
    if (strchr(name, '/') || name[1] == ':') {
        strncpy(out, name, PATH_MAX);
        return FileExists(out);
    }

    const char *pathEnv = getenv("PATH");
    if (!pathEnv) pathEnv = "";

    char pathCopy[256];
    strncpy(pathCopy, pathEnv, sizeof(pathCopy));

    char *rest = pathCopy;
    char *dir;

    while ((dir = strsep(&rest, ";")) != NULL) {
        if (dir[0] == '\0') continue;

        out[0] = '\0';
        strncat(out, dir, PATH_MAX - 1);
        strncat(out, "/", PATH_MAX - strlen(out) - 1);
        strncat(out, name, PATH_MAX - strlen(out) - 1);

        if (FileExists(out)) return 1;
    }

    return 0;
}

static char cwdBuf[PATH_MAX];

static char *GetCWDBuf() {
    getcwd(cwdBuf, PATH_MAX);
    return cwdBuf;
}

static void ShellExec(char *line) {
    char *argv[MAX_ARGS];
    int argc = 0;
    char *rest = line;
    char *tok;

    while ((tok = strsep(&rest, " \t")) != NULL) {
        if (tok[0] == '\0') continue;
        if (tok[0] == '$') {
            char *e = getenv(tok + 1);
            if (e)
                argv[argc++] = e;
            else
                argv[argc++] = tok;
        } else
            argv[argc++] = tok;
        if (argc >= MAX_ARGS - 1) break;
    }
    if (argc == 0) return;
    argv[argc] = NULL;

    if (strcmp(argv[0], "cd") == 0) {
        const char *target = argc > 1 ? argv[1] : "0:/";
        if (chdir(target) < 0)
            fprintf(FD_STDERR, "cd: %s: not found\n", target);
        return;
    }

    if (strcmp(argv[0], "pwd") == 0) {
        printf("%s\n", GetCWDBuf());
        return;
    }

    if (strcmp(argv[0], "exit") == 0) exit(0);

    if (strcmp(argv[0], "export") == 0) {
        if (argc != 3) {
            fprintf(FD_STDERR, "export: usage: export KEY VAL\n");
            return;
        }
        setenv(argv[1], argv[2], 1);
        printf("set: %s=%s\n", argv[1], argv[2]);
        return;
    }

    char resolved[PATH_MAX];

    if (PathLookup(argv[0], resolved))
        exec(resolved, argv);
    else
        fprintf(FD_STDERR, "shell: %s: not found\n", argv[0]);
}

int main(int argc, char **argv, char **envp) {
    if (!getenv("PATH")) setenv("PATH", "1:/bin", 1);

    printf("Welcome to cyShell\n");

    char line[256];
    while (1) {
        printf("%s $ ", GetCWDBuf());
        gets(line, sizeof(line));
        if (line[0]) ShellExec(line);
    }
    return 0;
}
