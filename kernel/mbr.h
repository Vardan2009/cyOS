#ifndef CY_MBR_H
#define CY_MBR_H

#include <stdint.h>

#include "blockdev.h"

#define PART_TYPE_EMPTY 0x00
#define PART_TYPE_FAT32 0x0B
#define PART_TYPE_FAT32_LBA 0x0C
#define PART_TYPE_FAT16 0x06
#define PART_TYPE_FAT16_S 0x04  // FAT16 < 32 MB
#define PART_TYPE_LINUX 0x83
#define PART_TYPE_EXTENDED 0x05

typedef struct {
    uint8_t bootIndicator;  // 80h = bootable
    uint8_t startCHS[3];
    uint8_t partitionType;
    uint8_t endCHS[3];
    uint32_t startLBA;
    uint32_t sectorCount;
} __attribute__((packed)) MBRPartitionEntry;

typedef struct {
    uint8_t bootstrap[446];
    MBRPartitionEntry partitions[4];
    uint16_t bootSignature;  // 0xaa55
} __attribute__((packed)) MBR;

int MBRRead(BlockDevice *dev, MBR *out);
void MBRPrint(MBR *mbr);

#endif  // CY_MBR_H
