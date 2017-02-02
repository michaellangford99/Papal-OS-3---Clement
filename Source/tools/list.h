#ifndef LIST_H
#define LIST_H

typedef struct node {
    uint32_t val;
    uint32_t* data;
    struct node* child;
    struct node* next;
}  __attribute__((packed)) node_t;

int list_add_node(node_t* head, uint32_t val);
int list_count_nodes(node_t* head);
void list_add_child(node_t* head, uint32_t val);
int list_remove_node(node_t** head, int index);
node_t* list_access_node(node_t* head, int index);
void list_print(node_t* head);
void list_move_to_end(node_t** head);
int test_list();

#endif