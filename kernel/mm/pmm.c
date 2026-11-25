#include <stdint.h>
#include <arch/multiboot_parser.h>
#include <kernel/tty.h>
#include <mm/pmm.h>


typedef struct {

} __attribute__((packed));

uint64_t usable_memory_low;
uint64_t usable_memory_high;

// 1 byte = 8 bit; toh 1 byte bitmap = 8 frames; 
uint64_t* frame_bitmap[1024];

// memory length % 4096 (4kb) = remainder; memory length - remainder -> completely divisble by 4kb frames
void pmm_init(){
            MEMORY_MAP_ENTRY mmap_entry = get_available_memory();
            uint64_t memory_length = mmap_entry.length - (mmap_entry.length % 4096);
            usable_memory_low = mmap_entry.base_address;
            usable_memory_high = usable_memory_low + memory_length;
            k_puts("total pages: ");
            k_puti(memory_length / 4096);
            k_puts(", bitmap-size: ");
            k_puti(memory_length/4096/8);
            k_putc('\n');
}

void pmm_alloc(){

}

void pmm_free(){
    
}