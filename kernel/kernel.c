#include <stdio.h>

#include "ata.h"
#include "gdt.h"
#include "idt.h"
#include "kmalloc.h"
#include "mbr.h"
#include "memory.h"
#include "multiboot.h"
#include "pci.h"
#include "pit.h"
#include "ps2.h"
#include "rtc.h"
#include "serial.h"
#include "vga.h"

void ATAInit() {
    static ATADrive ataDrives[4];
    const char *names[] = {"hda", "hdb", "hdc", "hdd"};

    ataDrives[0] = (ATADrive){0x1F0, 0x3F6, 0};
    ataDrives[1] = (ATADrive){0x1F0, 0x3F6, 1};
    ataDrives[2] = (ATADrive){0x170, 0x376, 0};
    ataDrives[3] = (ATADrive){0x170, 0x376, 1};

    for (int i = 0; i < 4; i++)
        if (ATADetect(&ataDrives[i]) == 0)
            ATACreateBlockdev(&ataDrives[i], names[i]);
}

void PCICallback(PCIDevice *dev) {
    printf("- PCI Device: Vendor: %x, Device: %x\n", dev->vendorID,
           dev->deviceID);
}

#include "fatfs/ff.h"

void FSTest() {
    FATFS fs;
    FIL file;
    BYTE buf[128];

    UINT bytesWritten, bytesRead;

    FRESULT res = f_mount(&fs, "0:", 1);
    if (res != FR_OK) {
        printf("Mount failed: %d\n", res);
        return;
    }

    f_open(&file, "0:/hello.txt", FA_CREATE_ALWAYS | FA_WRITE);
    f_write(&file, "Hello from cyOS!\n", 18, &bytesWritten);
    f_close(&file);

    f_open(&file, "0:/hello.txt", FA_READ);
    f_read(&file, buf, sizeof(buf) - 1, &bytesRead);
    buf[bytesRead] = '\0';
    f_close(&file);

    printf("Read: %s\n", buf);

    DIR dir;
    FILINFO fno;
    f_opendir(&dir, "0:/");
    while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0])
        printf("%s %d bytes\n", fno.fname, fno.fsize);
    f_closedir(&dir);
}

void kmain(uint32_t magic, MultibootInfo *mbi) {
    // enable SSE
    uint32_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 9) | (1 << 10);
    asm volatile("mov %0, %%cr4" ::"r"(cr4));

    SerialInit(DBGPORT);

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

    extern uint32_t _kernel_end;
    uint32_t physAllocStart =
        ((uint32_t)&_kernel_end - KERNEL_START + 0xFFF) & ~0xFFF;

    MemInit(mbi->memU * 1024, physAllocStart);

    KMallocInit(0x1000);
    MemMapPage(0xC00B8000, 0xB8000, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);

    DateTime dt = RTCGetDateTime();

    printf("%d/%d/%d %d:%d:%d\n", dt.day, dt.month, dt.year, dt.hour, dt.minute,
           dt.second);

    printf("Detecting ATA drives...\n");
    ATAInit();

    uint8_t sector[512];
    BlockDevice *bd = BlkDevFind("hda");
    BlkDevRead(bd, 0, 1, sector);

    MBR *mbr = (MBR *)&sector;

    MBRPrint(mbr);

    PCIEnumerate(PCICallback);

    /* for (int i = 0; i < 512; i += 16) {
        printf("%04x  ", i);

        for (int j = 0; j < 16; j++) printf("%02x ", sector[i + j]);

        printf(" ");

        for (int j = 0; j < 16; j++) {
            uint8_t c = sector[i + j];
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }

        printf("\n");
    } */

    FSTest();

    while (1) {
        char buf[512];

        printf("$ ");
        scanl(buf, 512);

        printf("==> %s\n", buf);
    }
}
