#ifndef CY_MBR_H
#define CY_MBR_H

#include <stdint.h>

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

void MBRPrint(MBR *mbr);

#endif  // CY_MBR_H
