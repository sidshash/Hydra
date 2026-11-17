#include <kernel/tty.h>


void kernel_main(void){
    
    k_set_color((uint8_t) VGA_COLOR_GREEN | VGA_COLOR_WHITE << 4);
    k_clear();
    k_puts("Welcome to Hydra!");
    while(1){

    }
}
