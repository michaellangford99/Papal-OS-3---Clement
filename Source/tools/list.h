#ifndef LIST_H
#define LIST_H

typedef struct node {
    uint32_t val;
    uint32_t* data;
    struct node* child;
    struct node* next;
}  __attribute__((packed)) node_t;

void list_add_node(node_t* head, uint32_t val);
int list_remove_node(node_t** head, int index);
node_t* list_access_node(node_t* head, int index);
void list_print(node_t* head);
int main();

#endif