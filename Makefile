CC := i686-elf-gcc
LD := i686-elf-ld
AS := nasm

ASFLAGS := -f elf32
CFLAGS  := -ffreestanding -masm=intel -O2 -Wall -Wextra -Ikernel/include -MMD -MP
LDFLAGS := -T kernel/linker.ld -nostdlib

ENTRY_OBJ := build/kernel/arch/boot.o

C_SRCS := kernel/main.c \
          $(wildcard kernel/core/*.c) \
          $(wildcard kernel/drivers/*.c) \
          $(wildcard kernel/mm/*.c) 

ASM_SRCS := $(filter-out kernel/arch/boot.asm, $(wildcard kernel/arch/*.asm))

OBJS := $(ENTRY_OBJ) \
		$(patsubst %.asm, build/%.o, $(ASM_SRCS)) \
		$(patsubst %.c, build/%.o, $(C_SRCS)) 
DEPS := $(OBJS:.o=.d)

KERNEL_ELF := build/kernel.elf
OS_ISO     := build/os.iso


.PHONY: all run clean

all: $(OS_ISO)

# Explicitly assemble NASM boot.asm
$(ENTRY_OBJ): kernel/arch/boot.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@ 

build/kernel/%.o: kernel/%.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Pattern rule for compiling C files
build/kernel/%.o: kernel/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link kernel.elf (ENTRY_OBJ is placed first)
$(KERNEL_ELF): $(OBJS) kernel/linker.ld
	$(LD) $(LDFLAGS) $(OBJS) -o $@

# Verify Multiboot header and build bootable GRUB ISO
$(OS_ISO): $(KERNEL_ELF) iso/boot/grub/grub.cfg
	@grub-file --is-x86-multiboot $(KERNEL_ELF) || (echo "Error: Not a valid Multiboot binary!" && exit 1)
	@mkdir -p build/isodir/boot/grub
	cp $(KERNEL_ELF) build/isodir/boot/kernel.elf
	cp iso/boot/grub/grub.cfg build/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ build/isodir

# Run in QEMU
run: $(OS_ISO)
	qemu-system-i386 -cdrom $(OS_ISO)

clean:
	rm -rf build

-include $(DEPS)

