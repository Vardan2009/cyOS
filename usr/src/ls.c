#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ansi.h"
#include "dirent.h"

static void FmtSize(uint32_t size, char *out) {
    if (size >= 1024 * 1024)
        snprintf(out, 16, "%u MB", size / (1024 * 1024));
    else if (size >= 1024)
        snprintf(out, 16, "%u KB", size / 1024);
    else
        snprintf(out, 16, "%u B ", size);
}

static DirEnt entries[512];
static int entryCount = 0;

static int CmpEntries(const void *a, const void *b) {
    const DirEnt *da = (const DirEnt *)a;
    const DirEnt *db = (const DirEnt *)b;

    if (da->isDir != db->isDir) return db->isDir - da->isDir;

    return strcmp(da->name, db->name);
}

static void DoLs(const char *path, int longFmt) {
    DIR *d = opendir(path);
    if (!d) {
        fprintf(FD_STDERR, "ls: cannot open directory: %s\n", path);
        return;
    }

    entryCount = 0;
    DirEnt *ent;
    while ((ent = readdir(d)) != NULL && entryCount < 512)
        entries[entryCount++] = *ent;
    closedir(d);

    qsort(entries, entryCount, sizeof(DirEnt), CmpEntries);

    for (int i = 0; i < entryCount; i++) {
        if (longFmt) {
            char size_str[16];
            if (entries[i].isDir)
                snprintf(size_str, 16, "     -");
            else
                FmtSize(entries[i].size, size_str);

            printf("%c  %6s  %s%s\n" ANSI_RESET, entries[i].isDir ? 'd' : '-',
                   size_str, entries[i].isDir ? ANSI_FG_BLUE ANSI_BOLD : "",
                   entries[i].name);
        } else {
            printf("%s%s" ANSI_RESET "%s\n",
                   entries[i].isDir ? ANSI_FG_BLUE ANSI_BOLD : "",
                   entries[i].name, entries[i].isDir ? "/" : "");
        }
    }
}

int main(int argc, char **argv) {
    int longFmt = 0;
    const char *path = ".";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0)
            longFmt = 1;
        else
            path = argv[i];
    }

    DoLs(path, longFmt);
    return 0;
}
