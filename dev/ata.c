#include "ata.h"

#include <io.h>
#include <stdio.h>
#include <string.h>

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

static void ATAFixString(uint8_t *str, int len) {
    for (int i = 0; i < len; i += 2) {
        uint8_t tmp = str[i];
        str[i] = str[i + 1];
        str[i + 1] = tmp;
    }
    for (int i = len - 1; i >= 0 && str[i] == ' '; i--) str[i] = '\0';
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

    uint16_t *dest = (uint16_t *)&drive->ident;
    for (int i = 0; i < 256; i++) dest[i] = inw(drive->base);

    ATAFixString(drive->ident.model, sizeof(drive->ident.model));
    ATAFixString(drive->ident.serial, sizeof(drive->ident.serial));
    ATAFixString(drive->ident.firmware, sizeof(drive->ident.firmware));

    drive->exists = 1;
    printf("ATA: %s (serial: %s)\n", drive->ident.model, drive->ident.serial);
    return 0;
}

static void ATASetupLBA48(ATADrive *drive, uint64_t lba, uint16_t sectors) {
    uint8_t select = drive->slave ? 0x50 : 0x40;
    outb(drive->base + 6, select);

    outb(drive->base + 1, 0x00);
    outb(drive->base + 2, (uint8_t)((sectors >> 8) & 0xFF));
    outb(drive->base + 3, (uint8_t)((lba >> 24) & 0xFF));
    outb(drive->base + 4, (uint8_t)((lba >> 32) & 0xFF));
    outb(drive->base + 5, (uint8_t)((lba >> 40) & 0xFF));

    outb(drive->base + 1, 0x00);
    outb(drive->base + 2, (uint8_t)(sectors & 0xFF));
    outb(drive->base + 3, (uint8_t)(lba & 0xFF));
    outb(drive->base + 4, (uint8_t)((lba >> 8) & 0xFF));
    outb(drive->base + 5, (uint8_t)((lba >> 16) & 0xFF));
}

static void ATASetupLBA28(ATADrive *drive, uint32_t lba, uint8_t sectors) {
    uint8_t headSelect = drive->slave ? 0xF0 : 0xE0;

    outb(drive->base + 1, 0x00);
    outb(drive->base + 2, sectors);
    outb(drive->base + 3, (uint8_t)(lba & 0xFF));
    outb(drive->base + 4, (uint8_t)((lba >> 8) & 0xFF));
    outb(drive->base + 5, (uint8_t)((lba >> 16) & 0xFF));
    outb(drive->base + 6, headSelect | ((lba >> 24) & 0x0F));
}

static inline int ATASupportsLBA48(ATADrive *drive) {
    return (drive->ident.commandsSupported[1] >> 10) & 1;
}

int ATARead(ATADrive *drive, uint32_t lba, uint8_t sectors, uint16_t *buf) {
    uint16_t *dest = (uint16_t *)buf;
    uint32_t done = 0;

    while (done < sectors) {
        if (ATASupportsLBA48(drive)) {
            uint16_t batch =
                (sectors - done > 65535) ? 65535 : (uint16_t)(sectors - done);
            ATASetupLBA48(drive, lba + done, batch);
            outb(drive->base + 7, 0x24);

            for (int s = 0; s < batch; s++) {
                if (ATAPoll(drive->base, drive->ctrl) < 0) return -1;
                for (int i = 0; i < 256; i++) *dest++ = inw(drive->base);
            }
            done += batch;
        } else {
            if (lba + done >= 0x10000000ULL) return -1;
            uint8_t batch =
                (sectors - done > 255) ? 255 : (uint8_t)(sectors - done);
            ATASetupLBA28(drive, (uint32_t)(lba + done), batch);
            outb(drive->base + 7, 0x20);

            for (int s = 0; s < batch; s++) {
                if (ATAPoll(drive->base, drive->ctrl) < 0) return -1;
                for (int i = 0; i < 256; i++) *dest++ = inw(drive->base);
            }
            done += batch;
        }
    }

    return 0;
}

int ATAWrite(ATADrive *drive, uint32_t lba, uint8_t sectors, uint16_t *buf) {
    uint16_t *src = (uint16_t *)buf;
    uint32_t done = 0;

    while (done < sectors) {
        if (ATASupportsLBA48(drive)) {
            uint16_t batch =
                (sectors - done > 65535) ? 65535 : (uint16_t)(sectors - done);
            ATASetupLBA48(drive, lba + done, batch);
            outb(drive->base + 7, 0x34);

            for (int s = 0; s < batch; s++) {
                if (ATAPoll(drive->base, drive->ctrl) < 0) return -1;
                for (int i = 0; i < 256; i++) outw(drive->base, *src++);
            }
            done += batch;
        } else {
            if (lba + done >= 0x10000000ULL) return -1;
            uint8_t batch =
                (sectors - done > 255) ? 255 : (uint8_t)(sectors - done);
            ATASetupLBA28(drive, (uint32_t)(lba + done), batch);
            outb(drive->base + 7, 0x30);

            for (int s = 0; s < batch; s++) {
                if (ATAPoll(drive->base, drive->ctrl) < 0) return -1;
                for (int i = 0; i < 256; i++) outw(drive->base, *src++);
            }
            done += batch;
        }
    }

    uint8_t flush_cmd = ATASupportsLBA48(drive) ? 0xEA : 0xE7;
    outb(drive->base + 7, flush_cmd);
    while (inb(drive->base + 7) & ATA_STATUS_BSY);

    return 0;
}

// blockdev registry
#include "blockdev.h"
#include "kmalloc.h"

static int ATABlkDevRead(BlockDevice *dev, uint64_t lba, uint32_t count,
                         void *buf) {
    ATADrive *drive = (ATADrive *)dev->driverData;
    uint32_t done = 0;
    while (done < count) {
        uint8_t batch = (count - done > 255) ? 255 : (uint8_t)(count - done);
        if (ATARead(drive, lba + done, batch, (uint16_t *)buf + done * 256) < 0)
            return -1;
        done += batch;
    }
    return 0;
}

static int ATABlkDevWrite(BlockDevice *dev, uint64_t lba, uint32_t count,
                          const void *buf) {
    ATADrive *drive = (ATADrive *)dev->driverData;
    uint32_t done = 0;
    while (done < count) {
        uint8_t batch = (count - done > 255) ? 255 : (uint8_t)(count - done);
        if (ATAWrite(drive, lba + done, batch, (uint16_t *)buf + done * 256) <
            0)
            return -1;
        done += batch;
    }
    return 0;
}

static int ATABlkDevFlush(BlockDevice *dev) {
    ATADrive *drive = (ATADrive *)dev->driverData;
    outb(drive->base + 7, 0xE7);
    while (inb(drive->base + 7) & ATA_STATUS_BSY);
    return 0;
}

uint64_t ATAGetLBA48Sectors(ATADrive *drive) {
    uint16_t *w = (uint16_t *)&drive->ident;
    return (uint64_t)w[100] | ((uint64_t)w[101] << 16) |
           ((uint64_t)w[102] << 32) | ((uint64_t)w[103] << 48);
}

BlockDevice *ATACreateBlockdev(ATADrive *drive, const char *name) {
    BlockDevice *dev = kmalloc(sizeof(BlockDevice));
    dev->name = name;
    dev->blockSize = 512;
    dev->present = drive->exists;
    dev->driverData = drive;
    dev->ops = (BlockDeviceOps){
        .read = ATABlkDevRead,
        .write = ATABlkDevWrite,
        .flush = ATABlkDevFlush,
    };

    dev->sectorCount = ATASupportsLBA48(drive) ? ATAGetLBA48Sectors(drive)
                                               : drive->ident.lba28Sectors;

    BlkDevRegister(dev);
    return dev;
}
