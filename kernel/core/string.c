#include <kernel/string.h>

void memset(void* starting_address, uint8_t value, size_t size){
    uint8_t* buf = starting_address;
    for(size_t i = 0; i < size; i++){
        buf[i] = value;
    }
}

void memcpy(void* dest, const void* src, size_t n){
    
}

