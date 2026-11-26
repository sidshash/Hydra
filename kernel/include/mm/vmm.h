#pragma once
#include <stdint.h>

void vmm_init();
uint64_t vmm_alloc();
void vmm_free();
