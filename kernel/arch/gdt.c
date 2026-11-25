#include <stdint.h>
#include <arch/gdt.h>
#include <scheduling/tss.h>
#define KERNEL_CODE_ACCESS_TYPE 1 << 7 | 0b00 << 5 | 1 << 4 | 1 << 3 | 0 << 2 | 1 << 1 | 1
#define KERNEL_DATA_ACCESS_TYPE 1 << 7 | 0b00 << 5 | 1 << 4 | 0 << 3 | 0 << 2 | 1 << 1 | 1
#define USER_CODE_ACCESS_TYPE 1 << 7 | 0b11 << 5 | 1 << 4 | 1 << 3 | 0 << 2 | 1 << 1 | 1
#define USER_DATA_ACCESS_TYPE 1 << 7 | 0b11 << 5 | 1 << 4 | 0 << 3 | 0 << 2 | 1 << 1 | 1
#define TSS_ACCESS_TYPE 0x89 

static GDT gdt;


static SEGMENT_DESCRIPTOR_8B create_segment_descriptor8B(uint32_t base, uint8_t type){
  return (SEGMENT_DESCRIPTOR_8B){
    .limit = 0xFFFF,
    .base_low = base,
    .base = base >> 16,
    .type = type,
    .segment = 0b1010,
    .base_high = base >> 24
  };
}
//base, type
static SEGMENT_DESCRIPTOR_16B create_segment_descriptor16B(uint64_t base, uint8_t type){
  return (SEGMENT_DESCRIPTOR_16B){
    .limit = 0xFFFF,
    .base_low = base,
    .base = base >> 16,
    .type = type,
    .segment = 0,
    .base_high = base >> 24,
    .base_higher = base >> 32
  };
}

struct __attribute__((packed)) GDTR{
  uint16_t size;
  uint64_t address;
};

void init_gdt(){
  uint64_t tss = get_tss();
  gdt.null = create_segment_descriptor8B(0, 0);
  gdt.kernel_code = create_segment_descriptor8B(0, KERNEL_CODE_ACCESS_TYPE);
  gdt.kernel_data = create_segment_descriptor8B(0, KERNEL_DATA_ACCESS_TYPE);
  gdt.user_code   = create_segment_descriptor8B(0, USER_CODE_ACCESS_TYPE);
  gdt.user_data   = create_segment_descriptor8B(0, USER_DATA_ACCESS_TYPE);
  gdt.tss         = create_segment_descriptor16B(tss, TSS_ACCESS_TYPE);
}

void load_gdt(){
  struct GDTR gdtr;
  gdtr.size = sizeof(gdt) - 1;
  gdtr.address = (uint64_t)&gdt;
  __asm__ volatile ("lgdt %0" :: "m"(gdtr));
}
