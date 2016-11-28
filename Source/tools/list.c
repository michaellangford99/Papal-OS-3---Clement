#include "../system.h"

//add node to beggining of list
void list_add_node(node_t* head, uint32_t val)
{
  node_t* current = head;
  while(true)
  {
    if (current->next == NULL)
    {
      //we're at the end of the list.
      current->next = (node_t*)kmalloc(sizeof(node_t));
      current->next->next = NULL;
      current->next->val = val;
      break;
    }
    current = current->next;
    
  }
}

int list_remove_node(node_t** head, int index)
{
  node_t* current = *head;
  if (index == 0)
  {
    node_t** temp = head;
    *head = (*head)->next;
    kfree((uint32_t*)*temp, sizeof(node_t));
    //head = newhead;
    return 1;
  }
  for (int i = 0; i < index; i++)
  {
    node_t* temp = current;
    current = current->next;
    
    if (i == index-1) //if this is the one we want to delete - current is what we want to delete
    {
      (temp->next) = (current->next);
      kfree((uint32_t*)current, sizeof(node_t));
      break;
    }
  }
  return 1;
}

node_t* list_access_node(node_t* head, int index)
{
  node_t* current = head;
  for (int i = 0; i < index; i++)
  {
    current = current->next;
    if (current == NULL)
      return NULL;
  }
  return current;
}

void list_print(node_t* head)
{
  node_t* current = head;
  while(true)
  {
    printf("%d\n", current->val);
    current = current->next;
    if (current == NULL)
    {
      //we're at the end of the list.
      break;
    }
  }
}

int main() {
    node_t* list = (node_t*)kmalloc(sizeof(node_t));
    list->val = 0;
    list->next = NULL;
    list_add_node(list, 1);
    list_add_node(list, 2);
    list_add_node(list, 3);
    list_add_node(list, 4);
    
    list_print(list);
    printf("\n");
    list_remove_node(&list, 4);
    list_print(list);
    printf("\n");
    
    list_print(list_access_node(list->next, 2));
    printf("\n");
  
    
    return 1;
}