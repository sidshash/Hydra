#include <arch/multiboot_parser.h>
#include <kernel/tty.h>
#include <mm/pmm.h>

#define MAX_BITMAP_SIZE 1024

extern uint64_t kernel_start[];
extern uint64_t kernel_end[];

uint64_t usable_memory_low;
uint64_t usable_memory_high;
// 1 byte = 8 bit; toh 1 byte bitmap = 8 frames; 
uint64_t frame_bitmap[MAX_BITMAP_SIZE] = {0};

static uint64_t address_to_frame(uint64_t address){
    return (address - usable_memory_low) / 4096;
}

static void set_frame_status(uint64_t frame_address, uint8_t status){
    uint64_t frame = address_to_frame(frame_address);
    uint64_t frame_index = frame / 64;
    uint8_t frame_bit = frame % 64;
    frame_bitmap[frame_index] &= ~(1ULL << frame_bit);
    frame_bitmap[frame_index] |= ((uint64_t)(1 & status) << frame_bit);
}

static void init_kernel_frames(){

            uint64_t* frame_pointer = kernel_start;
            while(frame_pointer <= kernel_end){
                set_frame_status((uint64_t)frame_pointer, 1);
                frame_pointer += 4096 / sizeof(uint64_t*);
            }
}

// memory length % 4096 (4kb) = remainder; memory length - remainder -> completely divisble by 4kb frames
void pmm_init(){
            MEMORY_MAP_ENTRY mmap_entry = get_available_memory();
            uint64_t memory_length = mmap_entry.length - (mmap_entry.length % 4096);
            usable_memory_low = mmap_entry.base_address;
            usable_memory_high = usable_memory_low + memory_length;
            init_kernel_frames();
}



uint64_t pmm_alloc(){
    for(uint64_t i = 0; i < MAX_BITMAP_SIZE; i++){
        if(frame_bitmap[i] == ~0ULL){
            continue;
        }

        uint8_t bit = __builtin_ctzll(~frame_bitmap[i]);

        uint64_t frame_address = 0x100000ULL + ((i * 64 + bit) * 4096);

        set_frame_status(frame_address, 1);

        return frame_address;
    }
}

void pmm_free(){

}

uint64_t pmm_get_kernel_size(){
    return kernel_end - kernel_start;
}