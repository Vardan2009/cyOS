#include <stdio.h>

#include "blockdev.h"
#include "datetime.h"
#include "fatfs/diskio.h"
#include "fatfs/ff.h"
#include "mbr.h"
#include "rtc.h"

static const char *driveNames[] = {"hda", "hdb", "hdc", "hdd"};
#define MAX_DRIVES 4

PARTITION VolToPart[FF_VOLUMES];
static uint32_t partitionLBAStart[FF_VOLUMES];

void FatFsRegisterPartitions(BYTE pdrv, BlockDevice *dev) {
    static int vol = 0;

    MBR mbr;
    if (MBRRead(dev, &mbr) < 0) {
        if (vol < FF_VOLUMES) {
            VolToPart[vol].pd = pdrv;
            VolToPart[vol].pt = 0;
            printf("Volume %d, drive %d (whole disk)\n", vol, pdrv);
            vol++;
        }
        return;
    }

    for (int i = 0; i < 4 && vol < FF_VOLUMES; i++) {
        MBRPartitionEntry *p = &mbr.partitions[i];
        if (p->partitionType == PART_TYPE_EMPTY) continue;
        if (p->partitionType == PART_TYPE_LINUX) continue;
        if (p->partitionType == PART_TYPE_EXTENDED) continue;

        VolToPart[vol].pd = pdrv;
        VolToPart[vol].pt = i + 1;
        printf("Volume %d, drive %d MBR partition %d\n", vol, pdrv, i + 1);
        vol++;
    }
}

static BlockDevice *get_drive(BYTE pdrv) {
    if (pdrv >= MAX_DRIVES) return NULL;
    return BlkDevFind(driveNames[pdrv]);
}

DSTATUS disk_initialize(BYTE pdrv) {
    BlockDevice *dev = get_drive(pdrv);
    if (!dev || !dev->present) return STA_NOINIT;
    return 0;
}

DSTATUS disk_status(BYTE pdrv) {
    BlockDevice *dev = get_drive(pdrv);
    if (!dev) return STA_NOINIT;
    if (!dev->present) return STA_NODISK;
    return 0;
}

DRESULT disk_read(BYTE pdrv, BYTE *buf, LBA_t sector, UINT count) {
    BlockDevice *dev = get_drive(pdrv);
    if (!dev) return RES_NOTRDY;

    if (BlkDevRead(dev, (uint64_t)sector, count, buf) < 0) return RES_ERROR;

    return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buf, LBA_t sector, UINT count) {
    BlockDevice *dev = get_drive(pdrv);
    if (!dev) return RES_NOTRDY;

    if (BlkDevWrite(dev, (uint64_t)sector, count, buf) < 0) return RES_ERROR;

    return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buf) {
    BlockDevice *dev = get_drive(pdrv);
    if (!dev) return RES_NOTRDY;

    switch (cmd) {
        case CTRL_SYNC:
            return BlkDevFlush(dev) == 0 ? RES_OK : RES_ERROR;

        case GET_SECTOR_COUNT:
            *(LBA_t *)buf = (LBA_t)dev->sectorCount;
            return RES_OK;

        case GET_SECTOR_SIZE:
            *(WORD *)buf = (WORD)dev->blockSize;
            return RES_OK;

        case GET_BLOCK_SIZE:
            *(DWORD *)buf = 1;
            return RES_OK;

        default:
            return RES_PARERR;
    }
}

DWORD get_fattime(void) {
    DateTime t = RTCGetDateTime();

    return ((DWORD)(t.year - 1980) << 25) | ((DWORD)t.month << 21) |
           ((DWORD)t.day << 16) | ((DWORD)t.hour << 11) |
           ((DWORD)t.minute << 5) | ((DWORD)t.second / 2);
}
