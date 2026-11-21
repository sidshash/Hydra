bits 32
; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
MBFLAGS  equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + MBFLAGS) ; checksum of above, to prove we are multiboot
                                ; CHECKSUM + MAGIC + MBFLAGS should be Zero (0)

; Declare a multiboot header that marks the program as a kernel. These are magic
; values that are documented in the multiboot standard. The bootloader will
; search for this signature in the first 8 KiB of the kernel file, aligned at a
; 32-bit boundary. The signature is in its own section so the header can be
; forced to be within the first 8 KiB of the kernel file.
section .multiboot
align 4
	dd MAGIC
	dd MBFLAGS
	dd CHECKSUM

; The multiboot standard does not define the value of the stack pointer register
; (esp) and it is up to the kernel to provide a stack. This allocates room for a
; small stack by creating a symbol at the bottom of it, then allocating 16384
; bytes for it, and finally creating a symbol at the top. The stack grows
; downwards on x86. The stack is in its own section so it can be marked nobits,
; which means the kernel file is smaller because it does not contain an
; uninitialized stack. The stack on x86 must be 16-byte aligned according to the
; System V ABI standard and de-facto extensions. The compiler will assume the
; stack is properly aligned and failure to align the stack will result in
; undefined behavior.
section .bss
align 4096
page_table_l4:
    resb 4096
page_table_l3:
    resb 4096
page_table_l2:
    resb 4096
align 16
stack_bottom:
resb 16384 ; 16 KiB is reserved for stack
stack_top:

section .rodata
gdt64:
    dq 0; null descriptor
    .code_segment: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53); code segment
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

; The linker script specifies _start as the entry point to the kernel and the
; bootloader will jump to this position once the kernel has been loaded. It
; doesn't make sense to return from this function as the bootloader is gone.
; Declare _start as a function symbol with the given symbol size.
section .text
global _start:
_start:
	; The bootloader has loaded us into 32-bit protected mode on a x86
	; machine. Interrupts are disabled. Paging is disabled. The processor
	; state is as defined in the multiboot standard. The kernel has full
	; control of the CPU. The kernel can only make use of hardware features
	; and any code it provides as part of itself. There's no printf
	; function, unless the kernel provides its own <stdio.h> header and a
	; printf implementation. There are no security restrictions, no
	; safeguards, no debugging mechanisms, only what the kernel provides
	; itself. It has absolute and complete power over the
	; machine.

	; To set up a stack, we set the esp register to point to the top of our
	; stack (as it grows downwards on x86 systems). This is necessarily done
	; in assembly as languages such as C cannot function without a stack.
	mov esp, stack_top

	; This is a good place to initialize crucial processor state before the
	; high-level kernel is entered. It's best to minimize the early
	; environment where crucial features are offline. Note that the
	; processor is not fully initialized yet: Features such as floating
	; point instructions and instruction set extensions are not initialized
	; yet. The GDT should be loaded here. Paging should be enabled here.
	; C++ features such as global constructors and exceptions will require
	; runtime support to work as well.

	; Enter the high-level kernel. The ABI requires the stack is 16-byte
	; aligned at the time of the call instruction (which afterwards pushes
	; the return pointer of size 4 bytes). The stack was originally 16-byte
	; aligned above and we've since pushed a multiple of 16 bytes to the
	; stack since (pushed 0 bytes so far) and the alignment is thus
	; preserved and the call is well defined.
        ; note, that if you are building on Windows, C functions may have "_" prefix in assembly: _kernel_main


    extern kernel_main
    call feature_test
    call setup_page_table
    jmp enable_long_mode
NO_LONG_MODE:
    hlt

NO_CPUID:
    hlt

; Checking availability of cpuid instruction
bits 32
feature_test:
    mov ebx, eax
    xor eax, 00200000h
    push eax
    popfd
    pushfd
    pop eax
    cmp eax, ebx
    jz NO_CPUID

    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb NO_LONG_MODE
    
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz NO_LONG_MODE
    ret 

setup_page_table:
    ; setup page table 4
    mov eax, page_table_l3
    or eax, 0b11
    mov [page_table_l4], eax

    ; setup page table 3
    mov eax, page_table_l2
    or eax, 0b11
    mov [page_table_l3], eax

    mov ecx, 0
    .loop:
        mov eax, 0x200000 
        mul ecx
        or eax, 0b10000011
        mov [page_table_l2 + ecx * 8], eax
        inc ecx
        cmp ecx, 512
        jne .loop
    ret


enable_long_mode:
    ; load gdt64
    lgdt [gdt64.pointer]
    ; setting cr4.pae=1 this is required for switching to long mode
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; setting up cr3 (page table base address)
    mov eax, page_table_l4
    mov cr3, eax

    ; enable long mode set efer.lme=1
    mov ecx, 0c0000080h; efer msr number
    rdmsr
    or eax, 1 << 8
    wrmsr

    ;enable paging to activate long mode set cr0.pg = 1
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    jmp gdt64.code_segment:.kernel_entry

bits 64
.kernel_entry:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    call kernel_main

	cli
.hang:	hlt
	jmp .hang
.end:

