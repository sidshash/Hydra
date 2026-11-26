#include <kernel/memset.h>

void k_memset(void* starting_address, uint8_t value, size_t size){
    uint8_t* buf = starting_address;
    for(size_t i = 0; i < size; i++){
        buf[i] = value;
    }
}
