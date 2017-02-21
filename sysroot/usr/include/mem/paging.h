#ifndef PAGING_H
#define PAGING_H


typedef struct {
  uint32_t page_table[1024];
} __attribute__((packed)) page_table_t;

int init_paging();

#endif