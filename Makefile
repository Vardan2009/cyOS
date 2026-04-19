CC       = gcc
ASM      = nasm
EMU      = qemu-system-x86_64
BUILDDIR = build

CINCLUDES = -Ilib -Ikernel -Idrivers -Iboot -Iarch/x86
CCFLAGS   = -m32 -fno-stack-protector -fno-builtin $(CINCLUDES)
DEPFLAGS  = -MMD -MP
ASMFLAGS  = -felf32
EMUFLAGS  =

C_SRCS   := $(wildcard kernel/*.c drivers/*.c lib/*.c arch/x86/*.c)
ASM_SRCS := $(wildcard boot/*.s) arch/x86/gdt.s
C_OBJS   := $(patsubst %.c, $(BUILDDIR)/%.c.o, $(C_SRCS))
ASM_OBJS := $(patsubst %.s, $(BUILDDIR)/%.s.o, $(ASM_SRCS))
OBJS     := $(C_OBJS) $(ASM_OBJS)
DEPS     := $(C_OBJS:.o=.d)

.PHONY: all clean emu

all: $(BUILDDIR)/cy.iso

$(BUILDDIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILDDIR)/%.s.o: %.s
	mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILDDIR)/cy.iso: $(OBJS)
	mkdir -p $(BUILDDIR)/image/boot/grub
	cp boot/grub/grub.cfg $(BUILDDIR)/image/boot/grub/grub.cfg
	ld -m elf_i386 -T boot/linker.ld -o $(BUILDDIR)/image/boot/kernel $(OBJS)
	grub-mkrescue -o $@ $(BUILDDIR)/image

clean:
	rm -rf $(BUILDDIR)

emu: $(BUILDDIR)/cy.iso
	$(EMU) $(EMUFLAGS) $(BUILDDIR)/cy.iso

-include $(DEPS)
