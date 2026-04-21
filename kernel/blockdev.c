#include "blockdev.h"

#include <string.h>

#define MAX_BLOCK_DEVICES 32

static BlockDevice *registry[MAX_BLOCK_DEVICES];
static int deviceCount = 0;

int BlkDevRegister(BlockDevice *dev) {
    if (deviceCount >= MAX_BLOCK_DEVICES) return -1;
    registry[deviceCount++] = dev;
    return 0;
}

BlockDevice *BlkDevGet(int index) {
    if (index < 0 || index >= deviceCount) return NULL;
    return registry[index];
}

BlockDevice *BlkDevFind(const char *name) {
    for (int i = 0; i < deviceCount; i++)
        if (strcmp(registry[i]->name, name) == 0) return registry[i];
    return NULL;
}

int BlkDevCount() { return deviceCount; }

int BlkDevRead(BlockDevice *dev, uint64_t lba, uint32_t count, void *buf) {
    if (!dev || !dev->present || !dev->ops.read) return -1;
    if (lba + count > dev->sectorCount) return -1;
    return dev->ops.read(dev, lba, count, buf);
}

int BlkDevWrite(BlockDevice *dev, uint64_t lba, uint32_t count,
                const void *buf) {
    if (!dev || !dev->present || !dev->ops.write) return -1;
    if (lba + count > dev->sectorCount) return -1;
    return dev->ops.write(dev, lba, count, buf);
}

int BlkDevFlush(BlockDevice *dev) {
    if (!dev || !dev->ops.flush) return 0;
    return dev->ops.flush(dev);
}
