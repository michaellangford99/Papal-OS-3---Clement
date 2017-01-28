#include "../system.h"

node_t* threads;

//the ebp and esp from interrupted process
uint32_t saved_esp;
uint32_t saved_ebp;

stack_t int_stack;

extern uint32_t stack_bottom;
extern uint32_t stack_top;

int pm_init()
{
  //allocate memory for interrupt stack
  int_stack.stack_bottom = (uint32_t)kmalloc(PM_INTERRUPT_STACK_SIZE);
  int_stack.stack_top = int_stack.stack_bottom + PM_INTERRUPT_STACK_SIZE;
  int_stack.stack_esp = int_stack.stack_top;
  
  //set up thread list, making first node the current process
  threads = (node_t*)kmalloc(sizeof(node_t));
  
  threads->val = 0;        //unused in pm.c
  threads->next = NULL;
  threads->child = NULL;   //unused in pm.c
  threads->data = (uint32_t*)kmalloc(sizeof(thread_t));    //pointer to thread descriptor struct
  thread_t* thread = (thread_t*)threads->data;
  
  thread->thread_id = PM_PROC_0_ID;
  thread->stack_top = stack_top;
  thread->stack_bottom = stack_bottom;
  
  return 0;
}