#ifndef DEBUG
#error "DEBUG is missing in vmm.c"
#endif
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <kernel/tty.h>
#include <kernel/memset.h>
#define MAX_PAGE_TABLE_SIZE 512
#define PTE_PRESENT   (1ULL << 0)
#define PTE_RW        (1ULL << 1)

uintptr_t __attribute__((aligned(4096))) pl4[MAX_PAGE_TABLE_SIZE] = {0};

extern uintptr_t kernel_start[];
extern uintptr_t kernel_end[];
extern uintptr_t KERNEL_VMA[];


static uintptr_t next(uintptr_t* current_table, uint16_t idx){
    if(!(current_table[idx] & PTE_PRESENT)){
        uint64_t frame = pmm_alloc();
        k_memset((void*)frame, 0, 4096);
        current_table[idx] = frame | PTE_PRESENT | PTE_RW;
    }
    return (uintptr_t) (current_table[idx] & ~0xfffULL);
}

void vmm_map_page(uintptr_t virtual_address, uintptr_t physical_address, uint8_t flags){
    uint16_t pl4i = (virtual_address >> 39) & 0x1FF;
    uint16_t pl3i = (virtual_address >> 30) & 0x1FF;
    uint16_t pl2i = (virtual_address >> 21) & 0x1FF;
    uint16_t pl1i = (virtual_address >> 12) & 0x1FF;

    uintptr_t* pl3 = (uintptr_t*)next(pl4, pl4i);
    uintptr_t* pl2 = (uintptr_t*)next(pl3, pl3i);
    uintptr_t* pl1 = (uintptr_t*)next(pl2, pl2i);

    pl1[pl1i] = physical_address | PTE_PRESENT | flags | PTE_RW ;
    
}

static void init_kernel_pages()
{
    uintptr_t phys = (uintptr_t)kernel_start & ~0xfffULL;
    uintptr_t end = ((uintptr_t)kernel_end + 0xfffULL) & ~0xfffULL;

    while (phys < end)
    {
        vmm_map_page(phys + (uintptr_t)KERNEL_VMA, phys, PTE_PRESENT | PTE_RW);
        phys += 0x1000ULL;
    }

    //vga buffer ka page bhi map karna hai kyuki wo 1mb se niche aata
    vmm_map_page((uintptr_t)0xb8000ULL + (uintptr_t)KERNEL_VMA, (uintptr_t)0xb8000ULL, PTE_PRESENT | PTE_RW);
}

void load_paging_table(){
    uintptr_t phys_addr = (uintptr_t)pl4 - (uintptr_t)KERNEL_VMA;
    __asm__ volatile ("mov cr3, %0" :: "r"(phys_addr): "memory");
    tty_set_buffer_ptr((uintptr_t)KERNEL_VMA);
}

void vmm_init(){
    init_kernel_pages();
    load_paging_table();
}
