#pragma once
#include <stddef.h>
#include <stdint.h>

void memset(void* starting_address, uint8_t value, size_t size);
void memcpy(void* dest, const void* src, size_t n);