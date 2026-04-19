#include "gdt.h"
#include "idt.h"
#include "pit.h"
#include "vga.h"

void kmain(void) {
    VGAReset();
    VGAPrint("Hello, cyOS!\n");

    GDTInit();
    VGAPrint("GDT Initialized\n");

    IDTInit();
    VGAPrint("IDT Initialized\n");

    PITInit();
    VGAPrint("PIT Initialized\n");

    while (1);
}
