#include "path.h"

#include <string.h>

int PathResolve(const char *cwd, const char *path, char *out) {
    char tmp[PATH_MAX];

    char vol[4] = {0};
    const char *cwdBody = cwd;
    if (cwd[1] == ':') {
        vol[0] = cwd[0];
        vol[1] = ':';
        cwdBody = cwd + 2;
    }

    const char *pathBody = path;
    if (path[1] == ':') {
        vol[0] = path[0];
        vol[1] = ':';
        pathBody = path + 2;
    }

    if (pathBody[0] == '/') {
        strncpy(tmp, pathBody, PATH_MAX);
        tmp[PATH_MAX - 1] = '\0';
    } else {
        tmp[0] = '\0';
        strncat(tmp, cwdBody, PATH_MAX - 1);
        strncat(tmp, "/", PATH_MAX - strlen(tmp) - 1);
        strncat(tmp, pathBody, PATH_MAX - strlen(tmp) - 1);
    }

    char parts[32][64];
    int depth = 0;
    char buf[PATH_MAX];
    strncpy(buf, tmp, PATH_MAX);
    char *tok = buf;
    char *seg;

    while ((seg = strsep(&tok, "/")) != NULL) {
        if (seg[0] == '\0' || strcmp(seg, ".") == 0) {
            continue;
        } else if (strcmp(seg, "..") == 0) {
            if (depth > 0) --depth;
        } else {
            if (depth >= 32) return -1;
            strncpy(parts[depth++], seg, 63);
        }
    }

    int pos = 0;
    for (int i = 0; vol[i]; i++) out[pos++] = vol[i];

    if (depth == 0)
        out[pos++] = '/';
    else {
        for (int i = 0; i < depth; i++) {
            out[pos++] = '/';
            for (int j = 0; parts[i][j] && pos < PATH_MAX - 1; j++)
                out[pos++] = parts[i][j];
        }
    }

    out[pos] = '\0';
    return 0;
}
