#include <stdio.h>

#include "gdt.h"
#include "idt.h"
#include "pit.h"
#include "ps2.h"
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

    PS2KBInit();
    printf("PS/2 Driver Initialized\n");

    while (1) {
        char buf[512];

        printf("$ ");
        scanl(buf, 512);

        printf("==> %s\n", buf);
    }
}
