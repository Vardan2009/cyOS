ENTRY(_start)

SECTIONS {
	. = 0x400000;

	.text : {
		lib/crt0.o(.text)
		*(.text)
	}

	.rodata : { *(.rodata) }
	.data   : { *(.data)   }
	.bss    : {
		*(.bss)
		*(COMMON)
	}
}
