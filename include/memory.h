#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

void memory_init(void);
void* kmalloc(size_t size);
void kfree(void* ptr);
void* get_free_page(void);
void free_page(void* page);
uint64_t mem_get_allocated(void);

#endif

