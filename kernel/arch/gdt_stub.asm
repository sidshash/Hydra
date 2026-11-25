global load_gdt:
lgdt [rdi];  first argument -> address of gdt 
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov fs, ax
mov gs, ax
