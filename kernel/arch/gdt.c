#pragma once

#include <stdint.h>


struct GDT_ENTRY{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_high;
    uint8_t access_bytes;
    uint8_t limit_and_flags;
    uint8_t base_higher;
    uint32_t base_highest;
    uint32_t reserved;
} __attributes__ ((packed));

struct GDT{
    uint16_t limit;
    uint16_t base;

} __attributes__((packed));
