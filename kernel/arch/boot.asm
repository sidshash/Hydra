bits 32

MAGIC        equ 0xE85250D6             ; Multiboot 2 magic number
ARCH         equ 0                      ; 0 = 32-bit protected mode i386
HEADER_LEN   equ header_end - header_start
CHECKSUM     equ -(MAGIC + ARCH + HEADER_LEN)

section .multiboot
align 8
header_start:
    dd MAGIC
    dd ARCH
    dd HEADER_LEN
    dd CHECKSUM

    ; End tag (required)
    dw 0    ; type = 0
    dw 0    ; flags = 0
    dd 8    ; size = 8
header_end:

section .boot.bss nobits alloc noexec write align=4096
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
multiboot_index:
    resb 4
multiboot_magic:
    resb 4

section .boot.rodata alloc noexec nowrite align=8
gdt64:
    dq 0; null descriptor
    .code_segment: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53); code segment
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

section .boot alloc exec nowrite
global _start:
extern kernel_main
_start:
    mov [multiboot_magic], eax
    mov [multiboot_index], ebx
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
    mov [page_table_l4 + 511 * 8], eax

    ; setup page table 3
    mov eax, page_table_l2
    or eax, 0b11
    mov [page_table_l3], eax
    mov [page_table_l3 + 510 * 8], eax

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
    mov rax, higher_half_entry
    jmp rax


section .text
higher_half_entry:
  mov rdi, [multiboot_magic]
  mov rsi, [multiboot_index]  

  mov rax, 0xFFFFFFFF80000000
  add rsp, rax

  call kernel_main
	cli
.hang:	hlt
	jmp .hang
.end:

