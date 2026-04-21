#ifndef CY_ATA_H
#define CY_ATA_H

#include <stdint.h>

typedef struct {
    uint16_t base;
    uint16_t ctrl;
    uint8_t slave;
    uint8_t exists;
    char model[41];
} ATADrive;

#define ATA_STATUS_ERR (1 << 0)
#define ATA_STATUS_DRQ (1 << 3)
#define ATA_STATUS_SRV (1 << 4)
#define ATA_STATUS_DF (1 << 5)
#define ATA_STATUS_RDY (1 << 6)
#define ATA_STATUS_BSY (1 << 7)

int ATADetect(ATADrive *drive);

#endif  // CY_ATA_H
