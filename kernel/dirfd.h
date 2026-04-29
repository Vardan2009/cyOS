#ifndef CY_DIRFD_H
#define CY_DIRFD_H

#include "fd.h"

int FDOpenDir(FileDescriptor *fd, const char *resolved);

#endif  // CY_DIRFD_H
