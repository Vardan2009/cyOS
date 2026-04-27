#include "env.h"

#include <kmalloc.h>
#include <string.h>

static char *EnvMakeEntry(const char *key, const char *value) {
    int len = strlen(key) + 1 + strlen(value) + 1;
    char *entry = kmalloc(len);
    strcpy(entry, key);
    strcat(entry, "=");
    strcat(entry, value);
    return entry;
}

void EnvSet(Process *proc, const char *key, const char *value) {
    int klen = strlen(key);

    for (int i = 0; i < proc->envc; i++) {
        if (strncmp(proc->envp[i], key, klen) == 0 &&
            proc->envp[i][klen] == '=') {
            kfree(proc->envp[i]);
            proc->envp[i] = EnvMakeEntry(key, value);
            return;
        }
    }

    if (proc->envc < PROCESS_MAX_ENV)
        proc->envp[proc->envc++] = EnvMakeEntry(key, value);
}

char *EnvGet(Process *proc, const char *key) {
    int klen = strlen(key);
    for (int i = 0; i < proc->envc; i++) {
        if (strncmp(proc->envp[i], key, klen) == 0 &&
            proc->envp[i][klen] == '=') {
            return proc->envp[i] + klen + 1;
        }
    }
    return NULL;
}

void EnvUnset(Process *proc, const char *key) {
    int klen = strlen(key);
    for (int i = 0; i < proc->envc; i++) {
        if (strncmp(proc->envp[i], key, klen) == 0 &&
            proc->envp[i][klen] == '=') {
            kfree(proc->envp[i]);
            for (int j = i; j < proc->envc - 1; j++)
                proc->envp[j] = proc->envp[j + 1];
            proc->envp[--proc->envc] = NULL;
            return;
        }
    }
}

void EnvInherit(Process *child, Process *parent) {
    child->envc = parent->envc;
    for (int i = 0; i < parent->envc; i++) {
        int len = strlen(parent->envp[i]) + 1;
        child->envp[i] = kmalloc(len);
        memcpy(child->envp[i], parent->envp[i], len);
    }
}
