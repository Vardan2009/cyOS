#ifndef CY_FILEFD_H
#define CY_FILEFD_H

#include <stdint.h>

#include "fd.h"

int FDOpenFile(FileDescriptor *fd, const char *path, uint8_t flags);

#endif  // CY_FILEFD_H
