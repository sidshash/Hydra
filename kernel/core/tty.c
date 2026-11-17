#include <kernel/tty.h>

static struct Char* buffer = (struct Char*) 0xB8000;
uint8_t row = 80;
uint8_t column = 20;

static uint8_t curr_color = VGA_COLOR_WHITE;
static uint8_t cursor = 0;

void k_putc(char c){
    buffer[cursor] = (struct Char){c, curr_color}; 
    cursor++;
}

void k_set_color(uint8_t color){
    curr_color = color;
}

void k_clear(){
    struct Char c = {' ', curr_color};

    for(size_t i = 0; i < row*column; i++){
        buffer[i] = c;
    }
}

void k_puts(char* s){
    size_t i = 0;
    while(s[i]){
        k_putc(s[i]);
        i++;
    }
}


