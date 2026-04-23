#include "mbr.h"

#include <stdio.h>

void MBRPrint(MBR *mbr) {
    printf("=== Master Boot Record ===\n");
    printf("- Boot Signature: %04x == aa55\n", mbr->bootSignature);

    printf("- Partitions\n");

    for (int i = 0; i < 4; ++i) {
        MBRPartitionEntry *part = &mbr->partitions[i];

        printf("   - %d | %s | Sector %d | Size %d\n", i,
               part->bootIndicator == 0x80 ? "BOOTABLE" : "--------",
               part->startLBA, part->sectorCount);
    }
}
