#pragma once
#include <stdint.h>

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
} __attribute((packed)) IDTR;

void idt_init();

void idt_load();
