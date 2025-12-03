#include "sync.h"
#include "types.h"

/* Spinlock implementation */
void spinlock_init(spinlock_t* lock) {
    lock->locked = 0;
}

void spinlock_lock(spinlock_t* lock) {
    while (__sync_lock_test_and_set(&lock->locked, 1)) {
        /* Spin */
        asm volatile("pause");
    }
}

void spinlock_unlock(spinlock_t* lock) {
    __sync_lock_release(&lock->locked);
}

/* Semaphore implementation */
void semaphore_init(semaphore_t* sem, int count) {
    sem->count = count;
    spinlock_init(&sem->lock);
}

void semaphore_wait(semaphore_t* sem) {
    spinlock_lock(&sem->lock);
    while (sem->count <= 0) {
        spinlock_unlock(&sem->lock);
        /* Yield and try again */
        asm volatile("pause");
        spinlock_lock(&sem->lock);
    }
    sem->count--;
    spinlock_unlock(&sem->lock);
}

void semaphore_signal(semaphore_t* sem) {
    spinlock_lock(&sem->lock);
    sem->count++;
    spinlock_unlock(&sem->lock);
}

/* Mutex implementation */
void mutex_init(mutex_t* mutex) {
    mutex->locked = 0;
    mutex->owner = -1;
    mutex->count = 0;
    mutex->wait_queue = NULL;
}

void mutex_lock(mutex_t* mutex) {
    /* Simple spinlock-based mutex for now */
    while (__sync_lock_test_and_set(&mutex->locked, 1)) {
        asm volatile("pause");
    }
    mutex->count++;
}

void mutex_unlock(mutex_t* mutex) {
    mutex->count--;
    __sync_lock_release(&mutex->locked);
}

