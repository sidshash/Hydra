#pragma once
#include <stdint.h>

typedef struct {
  uint16_t limit;
  uint16_t base_low;
  uint8_t base;
  uint8_t type;
  uint8_t segment;
  uint8_t base_high;
  uint32_t base_higher;
} __attribute__((packed)) SEGMENT_DESCRIPTOR_16B;

typedef struct {
  uint16_t limit;
  uint16_t base_low;
  uint8_t base;
  uint8_t type;
  uint8_t segment;
  uint8_t base_high;
} __attribute__((packed)) SEGMENT_DESCRIPTOR_8B;

typedef struct {
  SEGMENT_DESCRIPTOR_8B null;
  SEGMENT_DESCRIPTOR_8B kernel_code;
  SEGMENT_DESCRIPTOR_8B kernel_data;
  SEGMENT_DESCRIPTOR_8B user_code;
  SEGMENT_DESCRIPTOR_8B user_data;
  SEGMENT_DESCRIPTOR_16B tss;
} __attribute__ ((packed)) GDT;

void init_gdt();

void load_gdt();
