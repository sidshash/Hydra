#include <kernel/memset.h>

void memset(void* starting_address, uint32_t value, size_t size){
    unsigned char* address_ptr = (unsigned char*) starting_address;
    for(int i = 0; i < size; i++){
        address_ptr[i] = (unsigned char) value;
    }
}

