#include <stdio.h>

#include "gdt.h"
#include "idt.h"
#include "pit.h"
#include "vga.h"

void kmain(void) {
    VGAReset();

    printf("Hello, cyOS!\n");

    GDTInit();
    printf("GDT Initialized\n");

    IDTInit();
    printf("IDT Initialized\n");

    PITInit();
    printf("PIT Initialized\n");

    while (1);
}
