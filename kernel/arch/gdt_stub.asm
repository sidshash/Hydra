global load_gdt

load_gdt:
    lgdt [rdi];  first argument -> address of gdt 
    pop rax
    mov rcx, cs
    push rcx
    push rax
    retfq

.reload_segments:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    call .load_tss
    ret

.load_tss:
    mov ax, 0x28
    ltr ax
    ret
