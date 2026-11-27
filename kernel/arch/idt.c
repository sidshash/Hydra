#include <arch/idt.h>
#include <kernel/tty.h>

#define IDT_GATE_INTERRUPT 0b1 << 7 | 0b00 << 5 | 0b0 << 4 | 0xe
#define IDT_GATE_TRAP 0b1 << 7 | 0b00 << 5 | 0b0 << 4 | 0xf
#define IDT_GATE_INTERRUPT_RING3 0b1 << 7 | 0b11 << 5 | 0b0 << 4 | 0xe

////// list of stubs to map to idt ////////////
extern void idt_stub(void);
//////////////////////////////////////////////
extern void isr_stub_0(void);
extern void isr_stub_1(void);
extern void isr_stub_2(void);
extern void isr_stub_3(void);
extern void isr_stub_4(void);
extern void isr_stub_5(void);
extern void isr_stub_6(void);
extern void isr_stub_7(void);
extern void isr_stub_8(void);
extern void isr_stub_9(void);
extern void isr_stub_10(void);
extern void isr_stub_11(void);
extern void isr_stub_12(void);
extern void isr_stub_13(void);
extern void isr_stub_14(void);
extern void isr_stub_15(void);
extern void isr_stub_16(void);
extern void isr_stub_17(void);
extern void isr_stub_18(void);
extern void isr_stub_19(void);
extern void isr_stub_20(void);
extern void isr_stub_21(void);
extern void isr_stub_22(void);
extern void isr_stub_23(void);
extern void isr_stub_24(void);
extern void isr_stub_25(void);
extern void isr_stub_26(void);
extern void isr_stub_27(void);
extern void isr_stub_28(void);
extern void isr_stub_29(void);
extern void isr_stub_30(void);
extern void isr_stub_31(void);

static void(*isr_stubs[32]) = {
    isr_stub_0,
    isr_stub_1,
    isr_stub_2,
    isr_stub_3,
    isr_stub_4,
    isr_stub_5,
    isr_stub_6,
    isr_stub_7,
    isr_stub_8,
    isr_stub_9,
    isr_stub_10,
    isr_stub_11,
    isr_stub_12,
    isr_stub_13,
    isr_stub_14,
    isr_stub_15,
    isr_stub_16,
    isr_stub_17,
    isr_stub_18,
    isr_stub_19,
    isr_stub_20,
    isr_stub_21,
    isr_stub_22,
    isr_stub_23,
    isr_stub_24,
    isr_stub_25,
    isr_stub_26,
    isr_stub_27,
    isr_stub_28,
    isr_stub_29,
    isr_stub_30,
    isr_stub_31,
};
typedef struct
{
    uint64_t interrupt_number;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;

} __attribute__((packed)) INTERRUPT_FRAME;

typedef struct
{
    uint16_t offset_low;       // 0..15
    uint16_t segment_selector; // 16..31
    uint8_t ist;               // 32..34 ist & 35..39 reserved
    uint8_t gate_type;         // 40..43 gate_type & 44 0 && 45..46 dpl && 47 present
    uint16_t offset_high;
    uint32_t offset_higher;
    uint32_t resrved;

} __attribute__((packed)) IDT_ENTRY;

typedef struct
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) IDTR;

__attribute__((aligned(16))) static IDT_ENTRY idt[256] = {0};
static IDTR idtr = {sizeof(idt) - 1, (uint64_t)&idt};

void exception_handler(INTERRUPT_FRAME *interrupt_frame)
{
    k_puts("Interrupt with error: ");
    k_puti(interrupt_frame->error_code);
    k_putc('\n');
    k_puts("interrupt no.: ");
    k_puti(interrupt_frame->interrupt_number);
    k_putc('\n');
    while (1)
    {
        __asm__ volatile("cli; hlt");
    }
}

static void set_idt_descriptor(uint8_t entry, uint64_t isr, uint8_t gate)
{
    idt[entry].offset_low = isr;
    idt[entry].segment_selector = 0x08;
    idt[entry].ist = 0x00;
    idt[entry].gate_type = gate;
    idt[entry].offset_high = isr >> 16;
    idt[entry].offset_higher = isr >> 32;
    idt[entry].resrved = 0;
}

void idt_init()
{
    for (uint8_t entry = 0; entry < 32; entry++)
    {
        if (entry == 8 || entry == 10 || entry == 11 || entry == 12 ||
            entry == 13 || entry == 14 || entry == 17 || entry == 21)
        {
            set_idt_descriptor(entry, (uint64_t)isr_stubs[entry], IDT_GATE_INTERRUPT);
        }
        else
        {
            set_idt_descriptor(entry, (uint64_t)isr_stubs[entry], IDT_GATE_INTERRUPT);
        }
    }

    idt_load();
}

void idt_load()
{
    __asm__ volatile("lidt %0" ::"m"(idtr));
}
