#include <kernel/tty.h>
#define VGA_WIDTH 80 
#define VGA_HEIGHT 25
static struct Char* buffer = (struct Char*) 0xB8000;



static uint8_t curr_color = VGA_COLOR_WHITE;
static uint8_t curr_row = 0;
static uint8_t curr_col = 0;
void k_putc(char c){
    if(c == '\n'){
        curr_col = 0;
        curr_row++;
        if(curr_row == VGA_HEIGHT){
            curr_row == 0;
        }
        return;
    }

    buffer[curr_row * VGA_WIDTH + curr_col] = (struct Char){c, curr_color}; 

    curr_col++;
    if(curr_col == VGA_WIDTH){
        curr_col = 0;
        curr_row++;
        if(curr_row == VGA_HEIGHT){
            curr_row == 0;
        }
    }
}

void k_set_color(uint8_t color){
    curr_color = color;
}

void k_clear(){
    struct Char c = {' ', curr_color};

    for(size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++){
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


