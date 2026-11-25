
# Hydra

Hydra is a small x86-64 operating system kernel written in C and NASM. It starts from a Multiboot 2-compatible bootloader, switches from 32-bit protected mode to 64-bit long mode, maps the kernel into the higher half, and enters `kernel_main`.

The current code includes VGA text output, 64-bit page-table setup, a basic GDT implementation, TSS and IDT headers/source files, physical-memory-map parsing, and a preliminary physical-memory allocator.

> This is an educational, work-in-progress kernel. Several subsystems are incomplete or disabled, and it may not run under every virtual machine or emulator.

## Platform and requirements

The supported build path is Linux on an x86-64 host.

Required tools:

| Tool | Purpose |
| --- | --- |
| `x86_64-elf-gcc` | Compile freestanding C files for x86-64 |
| `x86_64-elf-ld` | Link the kernel ELF image |
| `nasm` | Assemble the 32-bit/64-bit boot code |
| `grub-mkrescue` | Create the bootable ISO |
| `grub-file` | Verify the kernel's Multiboot 2 header |
| `xorriso` | Build the ISO filesystem, used by `grub-mkrescue` |
| `qemu-system-x86_64` | Run the ISO in a virtual machine |

Do not use the host system GCC (`gcc`) or linker (`ld`) to build the kernel. A freestanding cross compiler avoids host-specific headers and target assumptions.

## Quick start

After installing the dependencies and building the cross compiler:

```sh
export PATH="$HOME/opt/cross/bin:$PATH"

git clone https://github.com/sidshash/Hydra.git
cd Hydra
make clean
make
make run
```

`make` creates:

- `build/kernel.elf` - linked ELF kernel
- `build/os.iso` - bootable GRUB ISO
- `build/isodir/` - staging directory used for the ISO

`make run` starts `build/os.iso` in QEMU. In the GRUB menu, select **Hydra**.

## Install dependencies

### Debian / Ubuntu

```sh
sudo apt update
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev \
  libmpfr-dev texinfo wget xz-utils nasm mtools xorriso \
  grub-pc-bin grub-common qemu-system-x86
```

The package name for GRUB utilities can differ on some distributions. Verify that all of these programs are available:

```sh
command -v nasm grub-mkrescue grub-file xorriso qemu-system-x86_64 make
```

For another distribution, install equivalent packages that provide:

- C/C++ build tools and Make
- Bison, Flex, GMP, MPC, MPFR, and Texinfo
- NASM
- GRUB 2 rescue tools
- Xorriso/Mtools
- QEMU x86-64

## Build an x86-64 cross compiler

The Makefile expects a cross compiler and linker named:

```text
x86_64-elf-gcc
x86_64-elf-ld
```

The following procedure builds Binutils and GCC from source under `$HOME/opt/cross`. It is based on the common OS-development cross-toolchain setup.

### 1. Choose source versions and directories

```sh
export PREFIX="$HOME/opt/cross"
export TARGET=x86_64-elf
export BINUTILS_VERSION=2.43.1
export GCC_VERSION=14.2.0
export PATH="$PREFIX/bin:$PATH"

mkdir -p "$HOME/src"
cd "$HOME/src"
```

The GNU package releases used here are known to work together for this build. You may replace the versions, but keep the Binutils and GCC source directories separate and ensure GCC supports the `x86_64-elf` target.

### 2. Download and extract Binutils and GCC

```sh
wget "https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz"
wget "https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz"

tar -xf "binutils-${BINUTILS_VERSION}.tar.xz"
tar -xf "gcc-${GCC_VERSION}.tar.xz"
```

If `wget` is unavailable, download the same `.tar.xz` files from [GNU Binutils](https://ftp.gnu.org/gnu/binutils/) or [GNU GCC](https://ftp.gnu.org/gnu/gcc/) and extract them with `tar -xf`.

### 3. Build and install Binutils

```sh
mkdir -p "$HOME/src/build-binutils"
cd "$HOME/src/build-binutils"

"$HOME/src/binutils-${BINUTILS_VERSION}/configure" \
  --target="$TARGET" \
  --prefix="$PREFIX" \
  --with-sysroot \
  --disable-nls \
  --disable-werror

make -j"$(nproc)"
make install
```

### 4. Build and install GCC

```sh
mkdir -p "$HOME/src/build-gcc"
cd "$HOME/src/build-gcc"

"$HOME/src/gcc-${GCC_VERSION}/configure" \
  --target="$TARGET" \
  --prefix="$PREFIX" \
  --disable-nls \
  --enable-languages=c \
  --without-headers

make -j"$(nproc)" all-gcc
make install-gcc

make -j"$(nproc)" all-target-libgcc
make install-target-libgcc
```

### 5. Verify the cross compiler

```sh
export PATH="$HOME/opt/cross/bin:$PATH"

x86_64-elf-gcc --version
x86_64-elf-ld --version
x86_64-elf-gcc -print-sysroot
```

If `-print-sysroot` prints an empty or unexpected value, verify that the GCC configuration used `--target=x86_64-elf` and that `$PREFIX/bin` is first in `PATH`.

To make the toolchain available in future shells, add this to `~/.bashrc` or `~/.zshrc`:

```sh
export PATH="$HOME/opt/cross/bin:$PATH"
```

Then open a new terminal or run:

```sh
source ~/.bashrc
```

## Build

From the repository root:

```sh
export PATH="$HOME/opt/cross/bin:$PATH"
make clean
make
```

Useful Make targets:

```sh
make          # Build build/kernel.elf and build/os.iso
make clean    # Remove the build directory
make run      # Build if necessary, then boot the ISO in QEMU
```

The build process is:

1. NASM assembles `kernel/arch/boot.asm` and other assembly files.
2. `x86_64-elf-gcc` compiles the C sources as freestanding 64-bit kernel code.
3. `x86_64-elf-ld` links the objects using `kernel/linker.ld`.
4. `grub-file` verifies the Multiboot 2 ELF kernel.
5. `grub-mkrescue` copies the kernel and GRUB configuration into an ISO.

Override the toolchain by changing the variables in `Makefile` if you installed the tools under different names or prefixes.

## Project layout

```text
Hydra/
├── Makefile                 # Build, ISO, clean, and QEMU targets
├── iso/boot/grub/grub.cfg   # GRUB Multiboot 2 menu
├── kernel/
│   ├── arch/                # Boot, GDT, IDT, port, and Multiboot code
│   ├── core/                # Core kernel services
│   ├── drivers/             # Device drivers
│   ├── include/             # Kernel headers
│   ├── mm/                  # Memory management
│   ├── scheduling/          # TSS and scheduling-related code
│   ├── linker.ld            # Kernel link layout
│   └── main.c               # Kernel entry point
└── build/                   # Generated ELF, ISO, objects, and ISO staging
```

## Cross-compiler and OS-development resources

- [Building a Cross Compiler](https://wiki.osdev.org/GCC_Cross-Compiler) - OSDev cross-GCC setup and explanations
- [Why Do I Need a Cross Compiler?](https://wiki.osdev.org/Why_do_I_need_a_Cross_Compiler) - host/compiler versus target/compiler concepts
- [Bare Bones](https://wiki.osdev.org/Bare_Bones) - freestanding kernel bootstrapping
- [Meaty Skeleton](https://wiki.osdev.org/Meaty_Skeleton) - organizing a freestanding kernel
- [Multiboot 2 specification](https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html)
- [GNU GRUB manual](https://www.gnu.org/software/grub/manual/grub/grub.html)
- [NASM documentation](https://www.nasm.us/docs.php)
- [QEMU documentation](https://www.qemu.org/documentation/)
- [GNU Binutils source](https://ftp.gnu.org/gnu/binutils/)
- [GNU GCC source](https://ftp.gnu.org/gnu/gcc/)
- [GNU GCC installation guide](https://gcc.gnu.org/install/)
