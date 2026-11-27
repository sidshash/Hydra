#include <scheduling/task.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <stdint.h>
#include <kernel/string.h>


typedef struct {
    uint64_t rip;
    uint64_t rsp;
    uint64_t ss;
    uint64_t kernel_rsp;
    uint64_t cr3;
} Task;

static Task tasks[1024] = {0};

void task_start(){
    //sabse pehle page table banayenge
    uintptr_t task_page_table = vmm_generate_task_page_table();
    tasks[0] = (Task){
        .cr3 = task_page_table,
    };
}

void user_task();