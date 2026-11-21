#include <kernel/tty.h>
#include <arch/idt.h>

void kernel_main(void){
    
    k_set_color((uint8_t) VGA_COLOR_GREEN | VGA_COLOR_WHITE << 4);
    k_clear();
    k_puts("Welcome to Hydra!\n");
    if(sizeof(void* ) == 8) {
        k_puts("Running in 64-bit Long Mode\n"); 
    }
    idt_init();
    idt_load();
}
