#include <arch/idt.h>
#include <kernel/tty.h>

#define IDT_GATE_INTERRUPT 0b1 << 7 | 0b00 << 5 | 0b0 << 4 | 0xe
#define IDT_GATE_TRAP 0b1 << 7 | 0b00 << 5 | 0b0 << 4 | 0xf
#define IDT_GATE_INTERRUPT_RING3 0b1 << 7 | 0b11 << 5 | 0b0 << 4 | 0xe

typedef struct {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__ ((packed)) INTERRUPT_FRAME;

typedef struct {
    uint16_t offset_low; //0..15
    uint16_t segment_selector; // 16..31
    uint8_t  ist; // 32..34 ist & 35..39 reserved
    uint8_t gate_type; // 40..43 gate_type & 44 0 && 45..46 dpl && 47 present
    uint16_t offset_high;
    uint32_t offset_higher;
    uint32_t resrved;

} __attribute__((packed)) IDT_ENTRY;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) IDTR;


__attribute__((aligned(16)))
static IDT_ENTRY idt[256] = {0};
static IDTR idtr = {sizeof(idt) - 1, (uint64_t) &idt};


__attribute__((interrupt))
static void exception_handler_error(INTERRUPT_FRAME* interrupt_frame, uint64_t error){
    k_puts("Interrupt with error triggered with error: ");
    k_puti(error);
    k_putc('\n');
    k_puts("rip: ");
    k_puti(interrupt_frame->rip);
    k_putc('\n');
    while(1){
        __asm__ volatile("cli; hlt");
    }
}

__attribute__((interrupt))
static void exception_handler_no_error(INTERRUPT_FRAME* interrupt_frame){
    k_puts("Interrupt without error triggered");
    while(1){
        __asm__ volatile("cli; hlt");
    }
}


static void set_idt_descriptor(uint8_t entry, uint64_t isr, uint8_t gate){
    idt[entry].offset_low = isr ;
    idt[entry].segment_selector = 0x08;
    idt[entry].ist = 0x00;
    idt[entry].gate_type = gate;
    idt[entry].offset_high = isr >> 16;
    idt[entry].offset_higher = isr >> 32;
    idt[entry].resrved = 0;
}



void idt_init(){
    for(uint8_t entry = 0; entry < 32; entry++){
        if(entry == 8 || entry == 10 || entry == 11 || entry == 12 ||
        entry == 13   || entry == 14 || entry == 17 || entry == 21){
            set_idt_descriptor(entry, (uint64_t) exception_handler_error, IDT_GATE_INTERRUPT);
        }
        else{
            set_idt_descriptor(entry, (uint64_t) exception_handler_no_error, IDT_GATE_INTERRUPT);
        }
    }
}

void idt_load(){
    __asm__ volatile("lidt %0" :: "m"(idtr));   
    __asm__ volatile("sti");
}
