#ifndef CY_ATA_H
#define CY_ATA_H

#include <stdint.h>

#include "blockdev.h"

typedef struct {
    uint16_t generalConfig;
    uint16_t obsolete1;
    uint16_t specificConfig;
    uint16_t obsolete2;
    uint16_t retired1[2];
    uint16_t obsolete3;
    uint16_t reserved1[2];
    uint16_t retired2;
    uint8_t serial[20];
    uint16_t retired3[2];
    uint16_t obsolete4;
    uint8_t firmware[8];
    uint8_t model[40];
    uint16_t maxSectorsPerTransfer;
    uint16_t reserved2;
    uint16_t capabilities[2];
    uint16_t obsolete5[2];
    uint16_t validityFlags;
    uint16_t obsolete6[5];
    uint16_t obsolete7;
    uint32_t lba28Sectors;
    uint16_t obsolete8;
    uint16_t multiwordDma;
    uint16_t pioModes;
    uint16_t minDmaCycleTime;
    uint16_t recDmaCycleTime;
    uint16_t minPioCycleTime;
    uint16_t minPioIordyCycleTime;
    uint16_t reserved3[6];
    uint16_t queueDepth;
    uint16_t sataCapabilities;
    uint16_t sataCapabilities2;
    uint16_t sataFeaturesSupported;
    uint16_t sataFeaturesEnabled;
    uint16_t majorVersion;
    uint16_t minorVersion;
    uint16_t commandsSupported[3];
    uint16_t commandsEnabled[3];
    uint16_t udmaModes;
    uint16_t obsolete9[2];
    uint16_t apmLevel;
    uint16_t masterPasswordId;
    uint16_t hwResetResult;
    uint16_t obsolete10;
    uint16_t streamMinSize;
    uint16_t streamTransferTime;
    uint16_t streamAccessLatency;
    uint32_t streamPerfGranularity;
    uint64_t lba48Sectors;
    uint16_t streamTransferTimePio;
    uint16_t maxDataSetMgmtLba;
    uint16_t physLogicalSector;
    uint16_t reserved4;
    uint64_t worldWideName;
    uint16_t reserved5[4];
    uint16_t obsolete11;
    uint32_t logicalSectorSize;
    uint16_t commandsSupported2[2];
    uint16_t reserved6[6];
    uint16_t obsolete12;
    uint16_t securityStatus;
    uint16_t vendor[31];
    uint16_t cfaPowerMode;
    uint16_t reserved7[7];
    uint16_t formFactor;
    uint16_t dataSetMgmt;
    uint8_t additionalProductId[8];
    uint16_t reserved8[2];
    uint8_t mediaSerial[60];
    uint16_t sctCommandTransport;
    uint16_t reserved9[2];
    uint16_t alignment;
    uint32_t wrvMode3Count;
    uint32_t wrvMode2Count;
    uint16_t obsolete13[3];
    uint16_t nominalSeekTime;
    uint16_t reserved10[2];
    uint16_t wrvMode;
    uint16_t reserved11;
    uint16_t transportMajor;
    uint16_t transportMinor;
    uint16_t reserved12[31];
    uint16_t integrity;
} __attribute__((packed)) ATAIdentify;

typedef struct {
    uint16_t base;
    uint16_t ctrl;
    uint8_t slave;
    uint8_t exists;
    ATAIdentify ident;
} ATADrive;

#define ATA_STATUS_ERR (1 << 0)
#define ATA_STATUS_DRQ (1 << 3)
#define ATA_STATUS_SRV (1 << 4)
#define ATA_STATUS_DF (1 << 5)
#define ATA_STATUS_RDY (1 << 6)
#define ATA_STATUS_BSY (1 << 7)

int ATADetect(ATADrive *drive);
int ATARead(ATADrive *drive, uint32_t lba, uint8_t sectors, uint16_t *buf);
int ATAWrite(ATADrive *drive, uint32_t lba, uint8_t sectors, uint16_t *buf);

BlockDevice *ATACreateBlockdev(ATADrive *drive, const char *name);

#endif  // CY_ATA_H
