MAKEFLAGS += -j$(shell nproc)
CC := x86_64-elf-gcc
LD := x86_64-elf-ld
AS := nasm

ASFLAGS := -f elf64
CFLAGS  := -DDEBUG -g -ffreestanding -masm=intel -mcmodel=kernel -fno-pic -fno-pie -mgeneral-regs-only -O2 -Wall -Wextra -Ikernel/include -MMD -MP
LDFLAGS := -T kernel/linker.ld -nostdlib

ENTRY_OBJ := build/kernel/arch/boot.o

C_SRCS := kernel/main.c \
          $(wildcard kernel/core/*.c) \
          $(wildcard kernel/drivers/*.c) \
          $(wildcard kernel/arch/*.c) \
          $(wildcard kernel/scheduling/*.c) \
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
	@grub-file --is-x86-multiboot2 $(KERNEL_ELF) || (echo "Error: Not a valid Multiboot2 binary!" && exit 1)
	@mkdir -p build/isodir/boot/grub
	cp $(KERNEL_ELF) build/isodir/boot/kernel.elf
	cp iso/boot/grub/grub.cfg build/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ build/isodir

# Run in QEMU
run: $(OS_ISO)
	qemu-system-x86_64 -cdrom $(OS_ISO)

clean:
	rm -rf build

-include $(DEPS)

