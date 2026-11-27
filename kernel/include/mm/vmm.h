#pragma once
#include <stdint.h>

void vmm_init();
void vmm_free();
uintptr_t vmm_generate_task_page_table();
