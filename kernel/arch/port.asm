global port_inb
global port_outb


port_inb:
    mov dx, [esp + 4]
    xor eax, eax
    in al, dx 
    ret

port_outb:
    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, al 
    ret

