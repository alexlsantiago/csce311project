#ifndef SYNC_H
#define SYNC_H

#include "types.h"

struct task;  /* Forward declaration */

typedef struct {
    volatile int locked;
} spinlock_t;

typedef struct {
    volatile int count;
    spinlock_t lock;
} semaphore_t;

typedef struct {
    volatile int locked;
    volatile int owner;
    volatile int count;
    struct task* wait_queue;
} mutex_t;

void spinlock_init(spinlock_t* lock);
void spinlock_lock(spinlock_t* lock);
void spinlock_unlock(spinlock_t* lock);

void semaphore_init(semaphore_t* sem, int count);
void semaphore_wait(semaphore_t* sem);
void semaphore_signal(semaphore_t* sem);

void mutex_init(mutex_t* mutex);
void mutex_lock(mutex_t* mutex);
void mutex_unlock(mutex_t* mutex);

#endif

