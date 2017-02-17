#ifndef PM_H
#define PM_H

#define PM_MAX_KERNEL_THREADS    255
#define PM_INTERRUPT_STACK_SIZE  65536 //64 KB
#define PM_PROC_0_ID             0
#define PM_THREAD_TIME_QUANTUM   10
#define PM_ESP_OFFSET            (-12*4)

typedef struct {
    uint32_t thread_id;
    struct x86_registers thread_regs;
    uint32_t quantum;
} thread_t;

typedef struct {
    uint32_t stack_esp;
    uint32_t stack_bottom;
    uint32_t stack_top;
}  __attribute__((packed))  stack_t;

int pm_init();
void proc_save(struct x86_registers* proc_regs);
int pm_new_thread(uint32_t* entry_point, uint32_t stack_size);
struct x86_registers* proc_schedule(struct x86_registers* proc_regs);

#endif