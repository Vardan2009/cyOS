CC       = clang
ASM      = nasm
LD       = clang
EMU      = qemu-system-x86_64
BUILDDIR = build

CINCLUDES = -Ilib -Ikernel -Idev -Iboot -Iarch/x86 -Imm
CCFLAGS   = -g -target i686-unknown-none-elf -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -m32 $(CINCLUDES)
DEPFLAGS  = -MMD -MP
ASMFLAGS  = -felf32
EMUFLAGS  = #-S -s
LDFLAGS = -target i686-unknown-none-elf -m32 -nostdlib -T boot/linker.ld -fuse-ld=lld -Wl,-z,notext

C_SRCS   := $(wildcard kernel/*.c dev/*.c lib/*.c arch/x86/*.c mm/*.c)
ASM_SRCS := $(wildcard boot/*.s arch/x86/*.s)
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
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/image/boot/kernel $(OBJS)
	grub-mkrescue -o $@ $(BUILDDIR)/image

clean:
	rm -rf $(BUILDDIR)

emu: $(BUILDDIR)/cy.iso
	$(EMU) $(EMUFLAGS) $(BUILDDIR)/cy.iso

-include $(DEPS)
