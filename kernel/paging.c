#include "paging.h"
#include "memory.h"
#include "types.h"

/* Simple identity paging for now */
/* In a full implementation, we'd have proper page tables */

void paging_init(void) {
    /* For now, we'll use identity mapping */
    /* RISC-V uses Sv39 paging, but for simplicity we'll skip it initially */
}

void* setup_page_table(void) {
    /* Allocate a page for page table */
    return get_free_page();
}

