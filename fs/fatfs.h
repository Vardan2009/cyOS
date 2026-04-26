#ifndef CY_FATFS_H
#define CY_FATFS_H

#include <stdint.h>

#include "blockdev.h"

void FatFsRegisterPartitions(BYTE pdrv, BlockDevice *dev);

#endif  // CY_FATFS_H
