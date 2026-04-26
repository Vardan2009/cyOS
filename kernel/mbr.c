#include "mbr.h"

#include <stdio.h>

int MBRRead(BlockDevice *dev, MBR *out) {
    if (BlkDevRead(dev, 0, 1, out) < 0) return -1;
    if (out->bootSignature != 0xAA55) return -1;
    return 0;
}

void MBRPrint(MBR *mbr) {
    printf("=== Master Boot Record ===\n");
    printf("- Boot Signature: %04x == aa55\n", mbr->bootSignature);

    printf("- Partitions\n");

    for (int i = 0; i < 4; ++i) {
        MBRPartitionEntry *part = &mbr->partitions[i];

        const char *typeStr = "UNKNOWN";

        switch (part->partitionType) {
            case PART_TYPE_EMPTY:
                typeStr = "EMPTY";
                break;
            case PART_TYPE_FAT32:
                typeStr = "FAT32";
                break;
            case PART_TYPE_FAT32_LBA:
                typeStr = "FAT32 (LBA)";
                break;
            case PART_TYPE_FAT16:
                typeStr = "FAT16";
                break;
            case PART_TYPE_FAT16_S:
                typeStr = "FAT16 (< 32MB)";
                break;
            case PART_TYPE_LINUX:
                typeStr = "LINUX";
                break;
            case PART_TYPE_EXTENDED:
                typeStr = "EXTENDED";
                break;
        }

        printf("   - %d | %s | Sector %d | Size %d | Type %s\n", i,
               part->bootIndicator == 0x80 ? "BOOTABLE" : "--------",
               part->startLBA, part->sectorCount, typeStr);
    }
}
