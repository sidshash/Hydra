#include <arch/multiboot_parser.h>
#include <kernel/tty.h>
typedef struct __attribute__((packed)){
    uint32_t size;
    uint32_t reserved;
} MULTIBOOT_HEADER;

typedef struct __attribute__((packed)){
    uint32_t type;
    uint32_t size;
} MULTIBOOT_TAG;

typedef struct __attribute__((packed)){
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
} MEMORY_MAP;


static MEMORY_MAP_ENTRY avilable_memory;

MEMORY_MAP_ENTRY get_available_memory(){
    return avilable_memory;
}


void parse_multiboot_structure(uint32_t multiboot_index){
    MULTIBOOT_HEADER* header = (MULTIBOOT_HEADER*) multiboot_index;
    header += 1;
    //loop entries till you find type 6 which is memory map
    while(((MULTIBOOT_TAG*)(header))->type != 6){
        MULTIBOOT_TAG* tag;
        tag = (MULTIBOOT_TAG*) header;
        header += (tag->size / 8) + 1;
    }

    MEMORY_MAP* mmap = (MEMORY_MAP*) header;
    uint32_t size = mmap->size;
    uint32_t entry_size = mmap->entry_size;

    MEMORY_MAP_ENTRY* mmap_entry = (MEMORY_MAP_ENTRY*)(mmap + 1);
    for(uint32_t i = 0; i < (size - 16) / entry_size; i++){
        if(mmap_entry->base_address >= 0x100000 && mmap_entry->type == 1){ //0x100000 (1Mib), type 1 = available, 2 = not available
            //log info of page which matches the criteria
            // multiple memory segments ko merge nahi kar raha. abhi aise hi rehne dete hain baadme dekhenge
            avilable_memory = *mmap_entry;
            return;// first segment use kar raha hai bass, baadme change karna hai
        }
        mmap_entry += 1;
    }

}
