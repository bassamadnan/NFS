// rwlock.h

#ifndef RWLOCK_H
#define RWLOCK_H

#include <pthread.h>
#include <semaphore.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t readers_cv;
    pthread_cond_t writers_cv;
    int readers;
    int writers;
    int active_writers;
} rw_lock_t;

void rw_lock_init(rw_lock_t *lock, sem_t *rwlock);
void rw_lock_acquire_read(rw_lock_t *lock);
void rw_lock_release_read(rw_lock_t *lock);
void rw_lock_acquire_write(rw_lock_t *lock);
void rw_lock_release_write(rw_lock_t *lock);

#endif  // RWLOCK_H
