#ifndef CY_ELF_H
#define CY_ELF_H

#include <stdint.h>

#include "process.h"

#define ELF_MAGIC 0x464C457F
#define ELF_CLASS_32 1
#define ELF_TYPE_EXEC 2
#define ELF_ARCH_X86 3

#define PT_LOAD 1
#define PF_X (1 << 0)
#define PF_W (1 << 1)
#define PF_R (1 << 2)

typedef struct {
    uint32_t magic;
    uint8_t class;
    uint8_t endian;
    uint8_t version;
    uint8_t abi;
    uint8_t _pad[8];
    uint16_t type;
    uint16_t arch;
    uint32_t elf_version;
    uint32_t entry;
    uint32_t ph_offset;
    uint32_t sh_offset;
    uint32_t flags;
    uint16_t eh_size;
    uint16_t ph_entry_size;
    uint16_t ph_count;
    uint16_t sh_entry_size;
    uint16_t sh_count;
    uint16_t sh_str_index;
} __attribute__((packed)) ELFHeader;

typedef struct {
    uint32_t type;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t fileSize;
    uint32_t memSize;
    uint32_t flags;
    uint32_t align;
} __attribute__((packed)) ELFProgramHeader;

Process *ELFLoad(const char *path);

#endif  // CY_ELF_H
