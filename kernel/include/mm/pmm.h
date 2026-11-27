#pragma once
#include <stdint.h>
void pmm_init();
uint64_t pmm_alloc();
void pmm_free();
uintptr_t pmm_get_usable_memory_high();
