#include "pci.h"

#include <io.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_VENDOR_ID 0x00    // 16
#define PCI_DEVICE_ID 0x02    // 16
#define PCI_COMMAND 0x04      // 16
#define PCI_STATUS 0x06       // 16
#define PCI_REVISION_ID 0x08  // 8
#define PCI_PROG_IF 0x09      // 8
#define PCI_SUBCLASS 0x0A     // 8
#define PCI_CLASS_CODE 0x0B   // 8
#define PCI_CACHE_LINE_SIZE 0x0C
#define PCI_LATENCY_TIMER 0x0D
#define PCI_HEADER_TYPE 0x0E  // 8
#define PCI_BIST 0x0F

#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1C
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24
#define PCI_SUBSYSTEM_VID 0x2C
#define PCI_SUBSYSTEM_ID 0x2E
#define PCI_INTERRUPT_LINE 0x3C
#define PCI_INTERRUPT_PIN 0x3D

#define PCI_CMD_IO_SPACE (1 << 0)
#define PCI_CMD_MEM_SPACE (1 << 1)
#define PCI_CMD_BUS_MASTER (1 << 2)
#define PCI_CMD_INT_DISABLE (1 << 10)

#define PCI_MAX_BUS 256
#define PCI_MAX_DEVICE 32
#define PCI_MAX_FUNC 8

static uint32_t PCIMakeAddr(uint8_t bus, uint8_t device, uint8_t func,
                            uint8_t offset) {
    return (uint32_t)(1 << 31) | ((uint32_t)bus << 16) |
           ((uint32_t)device << 11) | ((uint32_t)func << 8) | (offset & 0xFC);
}

uint32_t PCIRead32(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset) {
    outl(PCI_CONFIG_ADDRESS, PCIMakeAddr(bus, device, func, offset));
    return inl(PCI_CONFIG_DATA);
}

uint16_t PCIRead16(uint32_t bus, uint8_t device, uint8_t func, uint8_t offset) {
    outl(PCI_CONFIG_ADDRESS, PCIMakeAddr(bus, device, func, offset));
    return (uint16_t)((inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
}

uint8_t PCIRead8(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset) {
    outl(PCI_CONFIG_ADDRESS, PCIMakeAddr(bus, device, func, offset));
    return (uint8_t)((inl(PCI_CONFIG_DATA) >> ((offset & 3) * 8)) & 0xFF);
}

void PCIWrite32(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset,
                uint32_t value) {
    outl(PCI_CONFIG_ADDRESS, PCIMakeAddr(bus, device, func, offset));
    outl(PCI_CONFIG_DATA, value);
}

int PCIProbe(uint8_t bus, uint8_t dev, uint8_t func, PCIDevice *out) {
    uint16_t vendor = PCIRead16(bus, dev, func, PCI_VENDOR_ID);
    if (vendor == 0xFFFF) return 0;

    out->bus = bus;
    out->device = dev;
    out->func = func;
    out->vendorID = vendor;
    out->deviceID = PCIRead16(bus, dev, func, PCI_DEVICE_ID);
    out->classCode = PCIRead8(bus, dev, func, PCI_CLASS_CODE);
    out->subclass = PCIRead8(bus, dev, func, PCI_SUBCLASS);
    out->progIf = PCIRead8(bus, dev, func, PCI_PROG_IF);
    out->headerType = PCIRead8(bus, dev, func, PCI_HEADER_TYPE);
    out->irqLine = PCIRead8(bus, dev, func, PCI_INTERRUPT_LINE);
    out->irqPin = PCIRead8(bus, dev, func, PCI_INTERRUPT_PIN);

    for (int i = 0; i < 6; i++)
        out->bar[i] = PCIRead32(bus, dev, func, PCI_BAR0 + i * 4);

    return 1;
}

void PCIEnumerate(void (*callback)(PCIDevice *)) {
    for (uint16_t bus = 0; bus < PCI_MAX_BUS; bus++) {
        for (uint8_t dev = 0; dev < PCI_MAX_DEVICE; dev++) {
            PCIDevice d;
            if (!PCIProbe(bus, dev, 0, &d)) continue;
            callback(&d);

            if (d.headerType & 0x80)
                for (uint8_t fn = 1; fn < PCI_MAX_FUNC; fn++)
                    if (PCIProbe(bus, dev, fn, &d)) callback(&d);
        }
    }
}

#define BAR_TYPE_IO 0x01
#define BAR_MEM_TYPE 0x06
#define BAR_PREFETCH 0x08

PCIBarInfo PCIDecodeBar(uint8_t bus, uint8_t dev, uint8_t func, int barIdx) {
    PCIBarInfo info = {0};
    uint8_t offset = PCI_BAR0 + barIdx * 4;
    uint32_t bar = PCIRead32(bus, dev, func, offset);

    info.isIO = bar & BAR_TYPE_IO;

    if (info.isIO)
        info.base = bar & ~0x3;
    else
        info.base = bar & ~0xF;

    PCIWrite32(bus, dev, func, offset, 0xFFFFFFFF);
    uint32_t size_mask = PCIRead32(bus, dev, func, offset);
    PCIWrite32(bus, dev, func, offset, bar);  // restore

    if (info.isIO)
        size_mask &= ~0x3;
    else
        size_mask &= ~0xF;

    info.size = ~size_mask + 1;

    return info;
}

void PCIEnableDevice(uint8_t bus, uint8_t dev, uint8_t func) {
    uint16_t cmd = PCIRead16(bus, dev, func, PCI_COMMAND);
    cmd |= PCI_CMD_IO_SPACE | PCI_CMD_MEM_SPACE | PCI_CMD_BUS_MASTER;
    PCIWrite32(bus, dev, func, PCI_COMMAND, cmd);
}
