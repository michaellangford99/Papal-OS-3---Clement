#include <system.h>

uint32_t thread_count = 0;
node_t* threads;

//the ebp and esp from interrupted process
uint32_t saved_esp;
uint32_t saved_ebp;

stack_t int_stack;

//external references to addresses of proc 0 stack
extern uint32_t stack_bottom;
extern uint32_t stack_top;

struct x86_tss tss;

//lock_t proc_table_lock=LOCK_FREE;

int pm_init()
{
  printf("pm_init\n");
  //unlock(&proc_table_lock);
  //allocate memory for interrupt stack
  int_stack.stack_bottom = (uint32_t)kmalloc(PM_INTERRUPT_STACK_SIZE);
  int_stack.stack_top = int_stack.stack_bottom + PM_INTERRUPT_STACK_SIZE;
  int_stack.stack_esp = int_stack.stack_top;

  printf("....interrupt stack top   : %x\n", int_stack.stack_top);
  printf("....interrupt stack bottom: %x\n", int_stack.stack_bottom);
  printf("....interrupt stack esp   : %x\n", int_stack.stack_esp);

  //set up thread list, making first node the current process
  threads = (node_t*)kmalloc(sizeof(node_t));

  threads->val = 0;        //unused in pm.c
  threads->next = NULL;
  threads->child = NULL;   //unused in pm.c
  threads->data = (uint32_t*)kmalloc(sizeof(thread_t));    //pointer to thread descriptor struct
  thread_t* thread = (thread_t*)threads->data;

  thread->thread_id = thread_count;
  thread->quantum = PM_THREAD_TIME_QUANTUM;

  thread->thread_regs.esp = (uint32_t)&stack_top;
  thread->thread_regs.ebp = (uint32_t)&stack_bottom;

  //the registers of process 0 will be saved to the struct upon the next clock interrupt

  //set up user mode

  //tss = (struct x86_tss*)kmalloc(sizeof(struct x86_tss));

  printf("....TSS Address: %x\n", (uint32_t)&tss);

  memset((char*)&tss, 0, sizeof(struct x86_tss));

  tss.esp0 = int_stack.stack_esp;
  tss.ss0 = 0x00000010;

  //tss.esp0 = (uint32_t)kmalloc(1024);
  //set_memory_range_dpl(tss.esp0, 1023, DPL_0);

  load_TSS((uint32_t*)&tss, sizeof(struct x86_tss));

  return 0;
}

void save_ring_0_esp(uint32_t ring0_esp)
{
  //printf("TSS: esp0: 0x%x\n", ring0_esp);
  tss.esp0 = ring0_esp;
}

int pm_new_thread(uint32_t* entry_point, uint32_t stack_size, uint32_t privelege)
{
  //lock(&proc_table_lock);
  interrupt_block();
  //add new node to proc_list,
  int index = list_add_node(threads, 0);
  node_t* new_thread_node = list_access_node(threads, index);

  new_thread_node->data = (uint32_t*)kmalloc((stack_size < 4096) ? 4096 : stack_size);    //pointer to thread descriptor struct
  thread_t* thread = (thread_t*)new_thread_node->data;

  thread_count++;

  thread->thread_id = thread_count;

  thread->quantum = PM_THREAD_TIME_QUANTUM;

  //zero source & dest pointers
  thread->thread_regs.edi = 0;
  thread->thread_regs.esi = 0;

  //allocate stack
  thread->thread_regs.ebp = (uint32_t)kmalloc(4096);
  thread->thread_regs.esp = thread->thread_regs.ebp + stack_size - PM_ESP_OFFSET;
  thread->thread_regs.useresp = thread->thread_regs.esp;

  //zero general purpose registers
  thread->thread_regs.ebx = 0;
  thread->thread_regs.edx = 0;
  thread->thread_regs.ecx = 0;
  thread->thread_regs.eax = 0;

  //set cs:eip (important!!)
  thread->thread_regs.eip = (uint32_t)entry_point;
  //privelege level
  if (privelege == PM_PL0)
  {
    thread->thread_regs.cs = 0x00000008;
    thread->thread_regs.ss = 0x00000010;
    //set segments
    thread->thread_regs.gs = 0x00000010;
    thread->thread_regs.fs = 0x00000010;
    thread->thread_regs.es = 0x00000010;
    thread->thread_regs.ds = 0x00000010;
  }
  if (privelege == PM_PL3)
  {
    thread->thread_regs.cs = 0x0000001B;
    thread->thread_regs.ss = 0x00000023;
    //set segments
    thread->thread_regs.gs = 0x00000023;
    thread->thread_regs.fs = 0x00000023;
    thread->thread_regs.es = 0x00000023;
    thread->thread_regs.ds = 0x00000023;

    //stack will be clobbered otherwise by attempting to switch to an esp already in use because of TSS stack switch
    thread->int_stack_esp = kmalloc(4096);
    
    set_memory_range_dpl(thread->thread_regs.ebp, stack_size, DPL_3);
  }
  thread->thread_regs.eflags = 0x202;//?fv2

  //now that struct has correct registers, these must be copied onto the new stack
  uint32_t* thread_esp = (uint32_t*)(thread->thread_regs.esp - sizeof(struct x86_registers) + 28);//28 is offset of esp from struct start * 4
  memcpy((char*)thread_esp, (char*)(&(thread->thread_regs)), sizeof(struct x86_registers));
  /*
  thread_esp[0] = thread->thread_regs.ss;
  thread_esp[1] = thread->thread_regs.useresp;
  thread_esp[2] = thread->thread_regs.eflags;
  thread_esp[3] = thread->thread_regs.cs;
  thread_esp[4] = thread->thread_regs.eip;
  thread_esp[5] = thread->thread_regs.err_code;
  thread_esp[6] = thread->thread_regs.int_no;
  thread_esp[7] = thread->thread_regs.eax;
  thread_esp[8] = thread->thread_regs.ecx;
  thread_esp[9] = thread->thread_regs.edx;
  thread_esp[10] = thread->thread_regs.ebx;
  thread_esp[11] = thread->thread_regs.esp;
  thread_esp[12] = thread->thread_regs.ebp;
  thread_esp[13] = thread->thread_regs.esi;
  thread_esp[14] = thread->thread_regs.edi;
  thread_esp[15] = thread->thread_regs.ds;
  thread_esp[16] = thread->thread_regs.es;
  thread_esp[17] = thread->thread_regs.fs;
  thread_esp[18] = thread->thread_regs.gs;
  */

  printf("starting new proc: %d, with DPL %d\n", thread_count, privelege);
  /*
  printf("dumping registers of new proc: %d\n", thread_count);

  printf("gs:          0x%x\n", thread->thread_regs.gs);
  printf("fs:          0x%x\n", thread->thread_regs.fs);
  printf("es:          0x%x\n", thread->thread_regs.es);
  printf("ds:          0x%x\n", thread->thread_regs.ds);

  //zero source & dest pointers
  printf("edi:         0x%x\n", thread->thread_regs.edi);
  printf("esi:         0x%x\n", thread->thread_regs.esi);

  //allocate stack
  printf("ebp:         0x%x\n", thread->thread_regs.ebp);
  printf("esp:         0x%x\n", thread->thread_regs.esp);
  printf("useresp:     0x%x\n", thread->thread_regs.useresp);

  //zero general purpose registers
  printf("ebx:         0x%x\n", thread->thread_regs.ebx);
  printf("edx:         0x%x\n", thread->thread_regs.edx);
  printf("ecx:         0x%x\n", thread->thread_regs.ecx);
  printf("eax:         0x%x\n", thread->thread_regs.eax);

  //set cs:eip (important!!)
  printf("eip:         0x%x\n", thread->thread_regs.eip);
  printf("cs:          0x%x\n", thread->thread_regs.cs);
  printf("eflags:      0x%x\n", thread->thread_regs.eflags);
  printf("ss:          0x%x\n", thread->thread_regs.ss);
  */
  //unlock(&proc_table_lock);

  //_breakpoint();

  interrupt_unblock();
  return thread->thread_id;
}

void proc_save(struct x86_registers* proc_regs)
{

  //_breakpoint();

  thread_t* thread = (thread_t*)threads->data;

  //tss.esp0 = int_stack.stack_esp;
  //tss.ss0 = 0x00000010;
  /*thread->thread_regs.gs = proc_regs->gs;
  thread->thread_regs.fs = proc_regs->fs;
  thread->thread_regs.es = proc_regs->es;
  thread->thread_regs.ds = proc_regs->ds;
  thread->thread_regs.edi = proc_regs->edi;
  thread->thread_regs.esi = proc_regs->esi;
  thread->thread_regs.ebp = proc_regs->ebp;
  thread->thread_regs.esp = proc_regs->esp;
  thread->thread_regs.ebx = proc_regs->ebx;
  thread->thread_regs.edx = proc_regs->edx;
  thread->thread_regs.ecx = proc_regs->ecx;
  thread->thread_regs.eax = proc_regs->eax;
  thread->thread_regs.int_no = proc_regs->int_no;
  thread->thread_regs.err_code = proc_regs->err_code;
  thread->thread_regs.eip = proc_regs->eip;
  thread->thread_regs.cs = proc_regs->cs;
  thread->thread_regs.eflags = proc_regs->eflags;
  thread->thread_regs.useresp = proc_regs->useresp;
  thread->thread_regs.ss = proc_regs->ss;*/
  memcpy((char*)&thread->thread_regs, (char*)proc_regs, sizeof(struct x86_registers));

  //if dpl3
  if ((proc_regs->cs & 0xFF) == 0x1B)
  {
    save_ring_0_esp((uint32_t)thread->int_stack_esp);
  }
}

struct x86_registers* proc_schedule(struct x86_registers* proc_regs)
{
  if (((thread_t*)threads->data)->quantum>0)
    ((thread_t*)threads->data)->quantum--;

  /*if (lock(&proc_table_lock))
  {*/
    if (((thread_t*)threads->data)->quantum <= 0)
    {
      ((thread_t*)threads->data)->quantum = PM_THREAD_TIME_QUANTUM;
      list_move_to_end(&threads);
    }
    /*unlock(&proc_table_lock);
  }*/

  //get registers of process in list position 0
  thread_t* thread = (thread_t*)threads->data;
  //printf("thread_id: %d\n", thread->thread_id);

  //if dpl3
  if ((thread->thread_regs.cs & 0xFF) == 0x1B)
  {
    save_ring_0_esp((uint32_t)thread->int_stack_esp);
  }

  return (struct x86_registers*)(thread->thread_regs.esp + PM_ESP_OFFSET);
}
