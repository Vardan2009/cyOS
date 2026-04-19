#ifndef CY_MULTIBOOT_H
#define CY_MULTIBOOT_H

#include <stdint.h>

typedef struct {
    uint32_t tabsize;
    uint32_t strsize;
    uint32_t addr;
    uint32_t reserved;
} MultibootAoutSymbolTable;

typedef struct {
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
} MultibootElfSectionHeaderTable;

typedef struct {
    uint32_t flags;
    uint32_t memL;
    uint32_t memU;
    uint32_t bootdev;

    uint32_t cmdline;
    uint32_t modsCount;
    uint32_t modsAddr;

    union {
        MultibootAoutSymbolTable aoutSym;
        MultibootElfSectionHeaderTable elfSec;
    } u;

    uint32_t mmapLength;
    uint32_t mmapAddr;

    uint32_t drivesLength;
    uint32_t drivesAddr;

    uint32_t configTable;
    uint32_t bootLoaderName;

    uint32_t apmTable;

    uint32_t vbeControlInfo;
    uint32_t vbeModeInfo;
    uint16_t vbeMode;
    uint16_t vbeInterfaceSeg;
    uint16_t vbeInterfaceOff;
    uint16_t vbeInterfaceLen;
} MultibootInfo;

#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5

typedef struct {
    uint32_t size;
    uint32_t addrL;
    uint32_t addrH;
    uint32_t lenL;
    uint32_t lenH;
    uint32_t type;
} __attribute__((packed)) MultibootMMapEntry;

#endif  // CY_MULTIBOOT_H
