#include <scheduling/tss.h>
#include <kernel/tty.h>

typedef struct __attribute__((packed)) {
  uint32_t reserved;
  uint64_t rsp0;
  uint64_t rsp1;
  uint64_t rsp2;
  uint64_t reserved2;
  uint64_t ist1;
  uint64_t ist2;
  uint64_t ist3;
  uint64_t ist4;
  uint64_t ist5;
  uint64_t ist6;
  uint64_t ist7;
  uint64_t reserved3;
  uint16_t reserved4;
  uint16_t io_map_base_address;
} TSS;

static uint8_t __attribute__((aligned(16))) rsp0[16318];

static TSS tss;

void tss_init(){
  tss.rsp0 = (uint64_t)&rsp0;
}

uint64_t get_tss(){
  return (uint64_t)&tss;
}

