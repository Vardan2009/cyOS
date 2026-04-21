#include "ata.h"

#include <io.h>
#include <stdio.h>

static void ATA400nsDelay(uint16_t ctrl) {
    for (int i = 0; i < 4; ++i) inb(ctrl);
}

static int ATAPoll(uint16_t base, uint16_t ctrl) {
    ATA400nsDelay(ctrl);
    for (int i = 0; i < 100000; ++i) {
        uint8_t status = inb(ctrl);
        if (status & ATA_STATUS_ERR) return -1;
        if (status & ATA_STATUS_DF) return -1;
        if (!(status & ATA_STATUS_BSY) && (status & ATA_STATUS_DRQ)) return 0;
    }

    return -1;  // timeout
}

int ATADetect(ATADrive *drive) {
    outb(drive->base + 6, drive->slave ? 0xB0 : 0xA0);
    ATA400nsDelay(drive->ctrl);

    uint8_t status = inb(drive->base + 7);
    if (status == 0xFF) {
        drive->exists = 0;
        return -1;
    }

    outb(drive->base + 2, 0);
    outb(drive->base + 3, 0);
    outb(drive->base + 4, 0);
    outb(drive->base + 5, 0);
    outb(drive->base + 7, 0xEC);

    status = inb(drive->base + 7);
    if (status == 0) {
        drive->exists = 0;
        return -1;
    }

    while (inb(drive->base + 7) & ATA_STATUS_BSY);

    uint8_t mid = inb(drive->base + 4);
    uint8_t hi = inb(drive->base + 5);
    if (mid != 0 || hi != 0) {
        drive->exists = 0;
        return -1;  // atapi, not ata
    }

    if (ATAPoll(drive->base, drive->ctrl) < 0) {
        drive->exists = 0;
        return -1;
    }

    uint16_t identify[256];
    for (int i = 0; i < 256; i++) identify[i] = inw(drive->base);

    for (int i = 0; i < 20; i++) {
        drive->model[i * 2] = identify[27 + i] >> 8;
        drive->model[i * 2 + 1] = identify[27 + i] & 0xFF;
    }
    drive->model[40] = '\0';

    drive->exists = 1;
    printf("ATA drive: %s\n", drive->model);
    return 0;
}
