#include <scheduling/tss.h>
#include <kernel/tty.h>

static TSS tss;

void tss_init(){

 

}

uint64_t get_tss(){
  return (uint64_t)&tss;
}

