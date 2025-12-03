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

/* The actual heap */
static char heap[HEAP_SIZE];
static block_t* free_list = NULL;

static spinlock_t heap_lock;

/* Track allocated bytes */
static size_t allocated_bytes = 0;

/* Page pool (simple bump allocator) */
static void* page_pool = NULL;
static size_t page_pool_used = 0;

void memory_init(void) {
    /* Set up heap free list */
    free_list = (block_t*)heap;
    free_list->size = HEAP_SIZE - sizeof(block_t);
    free_list->next = NULL;
    free_list->free = 1;

    spinlock_init(&heap_lock);

    /* Initialize page pool */
    page_pool = (void*)0x90000000;         // Start of page pool
    page_pool_used = 0;
}

/* Align to 8 bytes */
static inline size_t align8(size_t x) {
    return (x + 7) & ~7;
}

void* kmalloc(size_t size) {
    if (size == 0) return NULL;

    size = align8(size);

    spinlock_lock(&heap_lock);

    block_t* cur = free_list;

    while (cur) {
        if (cur->free && cur->size >= size) {

            /* Split the block if large enough */
            if (cur->size >= size + sizeof(block_t) + 8) {
                block_t* new_block = (block_t*)((char*)cur + sizeof(block_t) + size);
                new_block->size = cur->size - size - sizeof(block_t);
                new_block->next = cur->next;
                new_block->free = 1;

                cur->next = new_block;
                cur->size = size;
            }

            cur->free = 0;
            allocated_bytes += cur->size;

            spinlock_unlock(&heap_lock);
            return (char*)cur + sizeof(block_t);
        }

        cur = cur->next;
    }

    spinlock_unlock(&heap_lock);
    return NULL; // Out of memory
}

void kfree(void* ptr) {
    if (!ptr) return;

    block_t* b = (block_t*)((char*)ptr - sizeof(block_t));

    spinlock_lock(&heap_lock);

    b->free = 1;
    allocated_bytes -= b->size;

    /* Merge with next block */
    if (b->next && b->next->free) {
        b->size += sizeof(block_t) + b->next->size;
        b->next = b->next->next;
    }

    /* Merge with previous block */
    block_t* cur = free_list;
    while (cur && cur->next != b) {
        cur = cur->next;
    }
    if (cur && cur->free) {
        cur->size += sizeof(block_t) + b->size;
        cur->next = b->next;
    }

    spinlock_unlock(&heap_lock);
}

/* Simple bump allocator for 4KB pages */
void* get_free_page(void) {
    spinlock_lock(&heap_lock);

    uintptr_t addr = (uintptr_t)page_pool + page_pool_used * PAGE_SIZE;

    if (addr + PAGE_SIZE >= 0x98000000) {
        spinlock_unlock(&heap_lock);
        return NULL; // Out of pages
    }

    page_pool_used++;

    void* page = (void*)addr;
    memset(page, 0, PAGE_SIZE);

    spinlock_unlock(&heap_lock);
    return page;
}

void free_page(void* page) {
    /* Not implemented (simple pool) */
    (void)page;
}

/* For shell "meminfo" */
size_t mem_get_allocated(void) {
    return allocated_bytes;
}
