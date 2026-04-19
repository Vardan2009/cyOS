OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS
{
	.     = 0x100000;

	.boot ALIGN(4) : { *(.multiboot) }
	.text ALIGN(4096) : { *(.text) }
	.data : { *(.data) }
	.bss  : { *(.bss)  }
}
