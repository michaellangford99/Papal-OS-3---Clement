#include "../system.h"

//add node to end of list
int list_add_node(node_t* head, uint32_t val)
{
  node_t* current = head;
  int index = 0;
  while(true)
  {
    if (current->next == NULL)
    {
      //we're at the end of the list.
      current->next = (node_t*)kmalloc(sizeof(node_t));
      current->next->next = NULL;
      current->next->val = val;
      current->next->child = NULL;
      current->next->data = NULL;
      break;
    }
    current = current->next;
    index++;
  }
  return index;
}

void list_add_child(node_t* head, uint32_t val)
{
  node_t* current = head;
  if (current->child == NULL)
  {
    current->child = (node_t*)kmalloc(sizeof(node_t));
    current->child->next = NULL;
    current->child->val = val;
    current->child->child = NULL;
    current->child->data = NULL;
    return;
  }
  current = current->child;
  while(true)
  {
    if (current->next == NULL)
    {
      //we're at the end of the list.
      current->next = (node_t*)kmalloc(sizeof(node_t));
      current->next->next = NULL;
      current->next->val = val;
      current->next->child = NULL;
      current->next->data = NULL;
      break;
    }
    current = current->child;
    
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

void list_print_children(node_t* head)
{
  node_t* current = head;
  while(true)
  {
    printf("%d\n", current->val);
    current = current->child;
    if (current == NULL)
    {
      //we're at the end of the list.
      break;
    }
  }
}

void list_print_tree(node_t* head)
{
  if (head == NULL)
  {
    return;
  }
  printf("-");
  node_t* root_tree = head;
  while(root_tree != NULL)
  {
    printf("%d", root_tree->val);
    list_print_tree(root_tree->child);
    root_tree = root_tree->next;
    printf("\f|\f");
    if (root_tree==NULL)
    {
      printf("\a");
      return;
    }
  }
}

int test_list() {
    printf("\n");
    node_t* list = (node_t*)kmalloc(sizeof(node_t));
    list->val = 0;
    list->next = NULL;
    list->child = NULL;
    list->data = NULL;
    list_add_node(list, 1);
    list_add_node(list, 2);
    list_add_node(list, 3);
    list_add_node(list, 4);
    list_add_child(list->next, 9);
    list_add_child(list->next->child, 9);
    list_add_node(list->next->child, 9);
    list_add_node(list->next->child->child, 9);
    
    list_print_tree(list);
    
    //list_print(list);
    //printf("\n");
    //list_remove_node(&list, 4);
    //list_print(list);
    //printf("\n");
    
    //list_print(list_access_node(list->next, 2));
    //printf("\n");
  
    
    return 1;
}