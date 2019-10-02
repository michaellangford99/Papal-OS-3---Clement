#ifndef PM_H
#define PM_H

#define PM_MAX_KERNEL_THREADS    255
#define PM_INTERRUPT_STACK_SIZE  65536 //64 KB
#define PM_PROC_0_ID             0
#define PM_THREAD_TIME_QUANTUM   10
#define PM_ESP_OFFSET            (-12*4)
#define PM_PL0                   0
#define PM_PL3                   3

typedef struct {
    uint32_t thread_id;
    struct x86_registers thread_regs;
    uint32_t quantum;
    uint32_t privilege;
    uint32_t* int_stack_esp
} thread_t;

typedef struct {
    uint32_t stack_esp;
    uint32_t stack_bottom;
    uint32_t stack_top;
}  __attribute__((packed))  stack_t;

int pm_init();
void save_ring_0_esp(uint32_t ring0_esp);
void proc_save(struct x86_registers* proc_regs);
int pm_new_thread(uint32_t* entry_point, uint32_t stack_size, uint32_t privelege);
uint32_t proc_get_privilege_active();
void proc_kill_active();
struct x86_registers* proc_schedule(struct x86_registers* proc_regs);

#endif
