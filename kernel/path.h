#ifndef CY_PATH_H
#define CY_PATH_H

#define PATH_MAX 256

int PathResolve(const char *cwd, const char *path, char *out);

#endif  // CY_PATH_H
