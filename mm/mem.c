#include "mem.h"

#include <stdio.h>

#include "multiboot.h"

void MemInit(MultibootInfo *mbi) {
    for (int i = 0; i < mbi->mmapLength; i += sizeof(MultibootMMapEntry)) {
        MultibootMMapEntry *entry = (MultibootMMapEntry *)(mbi->mmapAddr + i);

        printf(
            "=== Multiboot memory map entry %d\n\tLo addr: %x\n\tHi addr: "
            "%x\n\tLo "
            "Length: %x\n\tHi Length: %x\n\tSize: "
            "%x\n\tType: %d\n",
            i + 1, entry->addrL, entry->addrH, entry->lenL, entry->lenH,
            entry->size, entry->type);
    }
}
