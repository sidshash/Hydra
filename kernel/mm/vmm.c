#include <mm/vmm.h>
#include <mm/pmm.h>
#include <kernel/tty.h>
#include <kernel/string.h>
#define MAX_PAGE_TABLE_SIZE 512
#define PTE_PRESENT (1ULL << 0)
#define PTE_RW (1ULL << 1)
#define PTE_USER (1ULL << 2)

uintptr_t __attribute__((aligned(4096))) pl4[MAX_PAGE_TABLE_SIZE] = {0};

extern uintptr_t kernel_start[];
extern uintptr_t kernel_end[];
extern uintptr_t KERNEL_VMA[];

static uintptr_t next(uintptr_t *current_table, uint16_t idx)
{
    if (!(current_table[idx] & PTE_PRESENT))
    {
        uint64_t frame = pmm_alloc();
        memset((void *)(frame + (uintptr_t)KERNEL_VMA), 0, 4096);
        current_table[idx] = frame | PTE_PRESENT | PTE_RW;
    }
    return (uintptr_t)(current_table[idx] & ~0xfffULL);
}

void vmm_map_page(uintptr_t* table, uintptr_t virtual_address, uintptr_t physical_address, uint8_t flags)
{
    uint16_t pl4i = (virtual_address >> 39) & 0x1FF;
    uint16_t pl3i = (virtual_address >> 30) & 0x1FF;
    uint16_t pl2i = (virtual_address >> 21) & 0x1FF;
    uint16_t pl1i = (virtual_address >> 12) & 0x1FF;

    uintptr_t *pl3 = (uintptr_t *)(next(table, pl4i) + (uintptr_t)KERNEL_VMA);
    uintptr_t *pl2 = (uintptr_t *)(next(pl3, pl3i) + (uintptr_t)KERNEL_VMA);
    uintptr_t *pl1 = (uintptr_t *)(next(pl2, pl2i) + (uintptr_t)KERNEL_VMA);

    pl1[pl1i] = physical_address | flags;
}

static void init_kernel_pages()
{
    uintptr_t phys = (uintptr_t)kernel_start & ~0xfffULL;
    uintptr_t end = ((uintptr_t)kernel_end + 0xfffULL) & ~0xfffULL;

    while (phys < end)
    {
        vmm_map_page(pl4, phys + (uintptr_t)KERNEL_VMA, phys, PTE_PRESENT | PTE_RW);
        phys += 0x1000ULL;
    }

    // vga buffer ka page bhi map karna hai kyuki wo 1mb se niche aata
    vmm_map_page(pl4, (uintptr_t)0xb8000ULL + (uintptr_t)KERNEL_VMA, (uintptr_t)0xb8000ULL, PTE_PRESENT | PTE_RW);
}

void load_paging_table()
{
    uintptr_t phys_addr = (uintptr_t)pl4 - (uintptr_t)KERNEL_VMA;
    __asm__ volatile("mov cr3, %0" ::"r"(phys_addr) : "memory");
    tty_set_buffer_ptr((uintptr_t)KERNEL_VMA);
}

void init_hhdm_pages()
{
    uintptr_t start = ((uintptr_t)kernel_end + 0xfffULL) & ~0xfffULL;
    uintptr_t end = pmm_get_usable_memory_high();

    while (start < end)
    {
        vmm_map_page(pl4, start + (uintptr_t)KERNEL_VMA, start, PTE_PRESENT | PTE_RW);
        start += 0x1000ULL;
    }
}

void vmm_init()
{
    init_kernel_pages();
    init_hhdm_pages();
    load_paging_table();
}

static uintptr_t phys_to_virt(uintptr_t phys){
    return phys + (uintptr_t)KERNEL_VMA;
}

uintptr_t vmm_alloc(uintptr_t* table, uint16_t flags){
    uintptr_t frame = pmm_alloc();
    uintptr_t virt = phys_to_virt(frame);
    vmm_map_page((uintptr_t*)table, virt, frame, flags);
    return virt;
}

uintptr_t vmm_alloc_kernel_stack(){
    // 8KB stack = 2 page
    return vmm_alloc(pl4, PTE_PRESENT | PTE_RW );
}

uintptr_t vmm_alloc_user_stack(){

}

uintptr_t vmm_generate_task_page_table()
{
    uintptr_t task_pl4 = pmm_alloc(); // 4KB frame
    uintptr_t virt_pl4 = phys_to_virt(task_pl4);

    // clean newly allocated frame
    memset((void *)virt_pl4, 0, 4096);

    // pehle hi map karlo isse pehle kch khrab ho
    vmm_map_page(pl4, task_pl4, task_pl4, PTE_USER | PTE_RW | PTE_PRESENT);

    memcpy((void *)virt_pl4, pl4, sizeof(uintptr_t) * 512);
    return virt_pl4;
}
