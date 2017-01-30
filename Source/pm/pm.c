#include "../system.h"

node_t* threads;

uint32_t saved_esp;
uint32_t saved_ebp;

//the ebp and esp from interrupted process
stack_t int_stack;

//externl references to addresses of proc 0 stack
extern uint32_t stack_bottom;
extern uint32_t stack_top;

int pm_init()
{
  //allocate memory for interrupt stack
  int_stack.stack_bottom = (uint32_t)kmalloc(PM_INTERRUPT_STACK_SIZE);
  int_stack.stack_top = int_stack.stack_bottom + PM_INTERRUPT_STACK_SIZE;
  int_stack.stack_esp = int_stack.stack_top;
  
  printf("interrupt stack top: %d\n", int_stack.stack_top);
  printf("interrupt stack bottom: %d\n", int_stack.stack_bottom);
  printf("interrupt stack esp: %d\n", int_stack.stack_esp);
  
  //set up thread list, making first node the current process
  threads = (node_t*)kmalloc(sizeof(node_t));
  
  threads->val = 0;        //unused in pm.c
  threads->next = NULL;
  threads->child = NULL;   //unused in pm.c
  threads->data = (uint32_t*)kmalloc(sizeof(thread_t));    //pointer to thread descriptor struct
  thread_t* thread = (thread_t*)threads->data;
  
  thread->thread_id = PM_PROC_0_ID;
  thread->thread_regs.esp = stack_top;
  thread->thread_regs.ebp = stack_bottom;
  
  return 0;
}