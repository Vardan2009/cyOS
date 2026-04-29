CC       = clang
ASM      = nasm
LD       = clang
EMU      = qemu-system-i386
BUILDDIR = build

CINCLUDES = -Ilib -Ikernel -Idev -Iboot -Iarch/x86 -Imm -Ifs
CCFLAGS   = -g -target i686-unknown-none-elf -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -m32 $(CINCLUDES)
DEPFLAGS  = -MMD -MP
ASMFLAGS  = -felf32
EMUFLAGS  = -serial mon:stdio -display sdl #-S -s
LDFLAGS = -target i686-unknown-none-elf -m32 -nostdlib -T boot/linker.ld -fuse-ld=lld -Wl,-z,notext

C_SRCS   := $(wildcard kernel/*.c dev/*.c lib/*.c arch/x86/*.c mm/*.c fs/**/*.c fs/*.c)
ASM_SRCS := $(wildcard boot/*.s arch/x86/*.s kernel/*.s)
C_OBJS   := $(patsubst %.c, $(BUILDDIR)/%.c.o, $(C_SRCS))
ASM_OBJS := $(patsubst %.s, $(BUILDDIR)/%.s.o, $(ASM_SRCS))
OBJS     := $(C_OBJS) $(ASM_OBJS)
DEPS     := $(C_OBJS:.o=.d)

IMG      := $(BUILDDIR)/cy.img
IMG_SIZE := 64 # mb

DATADIR  := usr
IMG_SIZE := 1024

.PHONY: all clean emu

all: $(IMG)

$(BUILDDIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILDDIR)/%.s.o: %.s
	mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

$(IMG): $(OBJS)
	$(MAKE) -C $(DATADIR)
	mkdir -p $(BUILDDIR)/image/boot/grub
	cp boot/grub/grub.cfg $(BUILDDIR)/image/boot/grub/grub.cfg
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/image/boot/kernel $(OBJS)
	dd if=/dev/zero of=$(IMG) bs=1M count=$(IMG_SIZE)
	parted -s $(IMG) mklabel msdos
	parted -s $(IMG) mkpart primary fat32 1MiB 512MiB
	parted -s $(IMG) set 1 boot on
	parted -s $(IMG) mkpart primary fat32 512MiB 100%
	@LODEV=$$(sudo losetup -f --show -P $(IMG)); \
	sudo mkfs.fat -F32 $${LODEV}p1; \
	sudo mkfs.fat -F32 $${LODEV}p2; \
	sudo mkdir -p /mnt/cyimg; \
	sudo mount $${LODEV}p1 /mnt/cyimg; \
	sudo cp -r $(BUILDDIR)/image/. /mnt/cyimg/; \
	sudo grub-install --target=i386-pc \
	    --boot-directory=/mnt/cyimg/boot \
	    --no-floppy $${LODEV}; \
	sudo umount /mnt/cyimg; \
	sudo mkdir -p /mnt/cydata; \
	sudo mount $${LODEV}p2 /mnt/cydata; \
	sudo cp -r $(DATADIR)/. /mnt/cydata/; \
	sudo umount /mnt/cydata; \
	sudo losetup -d $${LODEV}

clean:
	$(MAKE) -C $(DATADIR) clean
	rm -rf $(BUILDDIR)

emu: $(IMG)
	$(EMU) $(EMUFLAGS) -drive file=$(IMG),format=raw,media=disk

-include $(DEPS)
