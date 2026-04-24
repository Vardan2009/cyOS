#ifndef CY_PCI_H
#define CY_PCI_H

#include <stdint.h>

typedef struct {
    uint8_t bus, device, func;
    uint16_t vendorID, deviceID;
    uint8_t classCode, subclass, progIf;
    uint8_t headerType;
    uint8_t irqLine, irqPin;
    uint32_t bar[6];
} PCIDevice;

typedef struct {
    uint32_t base;
    uint32_t size;
    int isIO;
} PCIBarInfo;

int PCIProbe(uint8_t bus, uint8_t dev, uint8_t func, PCIDevice *out);
void PCIEnumerate(void (*callback)(PCIDevice *));
PCIBarInfo PCIDecodeBar(uint8_t bus, uint8_t dev, uint8_t func, int barIdx);
void PCIEnableDevice(uint8_t bus, uint8_t dev, uint8_t func);

#endif  // CY_PCI_H
