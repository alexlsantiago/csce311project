#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>

/* uintptr_t is already defined in stdint.h for GCC, but we ensure: */
typedef uint64_t uintptr_t;

/* Boolean type */
typedef enum { false = 0, true = 1 } bool;

/* Page size */
#define PAGE_SIZE 4096

#endif
