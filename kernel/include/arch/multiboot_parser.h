#pragma once
#include <stdint.h>

typedef struct __attribute__((packed)){
    uint64_t base_address;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} MEMORY_MAP_ENTRY;

void parse_multiboot_structure(uint32_t multiboot_index);
MEMORY_MAP_ENTRY get_available_memory();