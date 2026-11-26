#include <kernel/tty.h>
#include <arch/idt.h>
#include <scheduling/tss.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <arch/multiboot_parser.h>


void kernel_main(uint32_t multiboot_magic, uint32_t multiboot_index){

    
    parse_multiboot_structure(multiboot_index);

    pmm_init();
    vmm_init();
    
    idt_init();
    idt_load();
    tss_init();
    //set color and clear screen before printing anything
    k_set_color((uint8_t) VGA_COLOR_GREEN | VGA_COLOR_WHITE << 4);
    k_clear();
    
    k_puts("Welcome to Hydra!\n");

    if(sizeof(void* ) == 8) {
        k_puts("Running in 64-bit Long Mode\n"); 
    }
    
}
