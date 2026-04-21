#ifndef CY_BLOCKDEV_H
#define CY_BLOCKDEV_H

#include <stdint.h>

typedef struct BlockDevice BlockDevice;

typedef struct {
    int (*read)(BlockDevice *dev, uint64_t lba, uint32_t count, void *buf);
    int (*write)(BlockDevice *dev, uint64_t lba, uint32_t count,
                 const void *buf);

    // optional
    int (*flush)(BlockDevice *dev);
} BlockDeviceOps;

struct BlockDevice {
    const char *name;
    BlockDeviceOps ops;

    uint32_t sectorCount;
    uint32_t blockSize;
    uint8_t present;
    void *driverData;
};

int BlkDevRegister(BlockDevice *dev);
BlockDevice *BlkDevGet(int index);
BlockDevice *BlkDevFind(const char *name);

int BlkDevCount();

int BlkDevRead(BlockDevice *dev, uint64_t lba, uint32_t count, void *buf);
int BlkDevWrite(BlockDevice *dev, uint64_t lba, uint32_t count,
                const void *buf);
int BlkDevFlush(BlockDevice *dev);

#endif  // CY_BLOCKDEV_H
