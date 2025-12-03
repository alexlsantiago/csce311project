#include "memory.h"
#include "kernel.h"
#include "sync.h"
#include "string.h"
#include "types.h"

#define MEMORY_START 0x80400000
#define MEMORY_END   0x88000000
#define HEAP_SIZE    (MEMORY_END - MEMORY_START)

typedef struct block {
    struct block* next;
    size_t size;
    int free;
} block_t;

static char heap[HEAP_SIZE];
static block_t* free_list = NULL;
static spinlock_t heap_lock;

static void* page_pool = NULL;
static int page_pool_size = 0;
static spinlock_t page_lock;

void memory_init(void) {
    /* Initialize heap */
    free_list = (block_t*)heap;
    free_list->size = HEAP_SIZE - sizeof(block_t);
    free_list->next = NULL;
    free_list->free = 1;
    
    spinlock_init(&heap_lock);
    spinlock_init(&page_lock);
    
    /* Initialize page pool */
    page_pool = (void*)0x90000000;  /* Start of page pool */
    page_pool_size = 0;
}

void* kmalloc(size_t size) {
    if (size == 0) return NULL;
    
    /* Align to 8 bytes */
    size = (size + 7) & ~7;
    
    spinlock_lock(&heap_lock);
    
    block_t* current = free_list;
    block_t* prev = NULL;
    
    while (current) {
        if (current->free && current->size >= size) {
            /* Found a block */
            if (current->size >= size + sizeof(block_t) + 8) {
                /* Split block */
                block_t* new_block = (block_t*)((char*)current + sizeof(block_t) + size);
                new_block->size = current->size - size - sizeof(block_t);
                new_block->next = current->next;
                new_block->free = 1;
                current->next = new_block;
                current->size = size;
            }
            
            current->free = 0;
            spinlock_unlock(&heap_lock);
            return (void*)((char*)current + sizeof(block_t));
        }
        prev = current;
        current = current->next;
    }
    
    spinlock_unlock(&heap_lock);
    return NULL;  /* Out of memory */
}

void kfree(void* ptr) {
    if (!ptr) return;
    
    block_t* block = (block_t*)((char*)ptr - sizeof(block_t));
    
    spinlock_lock(&heap_lock);
    
    block->free = 1;
    
    /* Try to merge with next block */
    if (block->next && block->next->free) {
        block->size += sizeof(block_t) + block->next->size;
        block->next = block->next->next;
    }
    
    /* Try to merge with previous block */
    block_t* current = free_list;
    block_t* prev = NULL;
    while (current) {
        if (current->next == block && current->free) {
            current->size += sizeof(block_t) + block->size;
            current->next = block->next;
            spinlock_unlock(&heap_lock);
            return;
        }
        prev = current;
        current = current->next;
    }
    
    spinlock_unlock(&heap_lock);
}

void* get_free_page(void) {
    spinlock_lock(&page_lock);
    
    void* page = (void*)((uintptr_t)page_pool + page_pool_size * PAGE_SIZE);
    page_pool_size++;
    
    if ((uintptr_t)page >= 0x98000000) {
        spinlock_unlock(&page_lock);
        return NULL;  /* Out of pages */
    }
    
    /* Zero the page */
    memset(page, 0, PAGE_SIZE);
    
    spinlock_unlock(&page_lock);
    return page;
}

void free_page(void* page) {
    /* Simple implementation - just mark as free */
    /* In a real OS, we'd track free pages */
    (void)page;
}

