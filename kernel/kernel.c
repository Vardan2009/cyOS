#include "gdt.h"
#include "vga.h"

void kmain(void) {
    VGAReset();
    VGAPrint("Hello, cyOS!\r\n");

    GDTInit();

    VGAPrint("GDT Initialized!\r\n");
}
