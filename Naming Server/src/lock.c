#include "../inc/lock.h"

void rw_lock_init(rw_lock_t *lock, sem_t *rwlock) {
    sem_init(rwlock, 0, 1);
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->readers_cv, NULL);
    pthread_cond_init(&lock->writers_cv, NULL);
    lock->readers = 0;
    lock->writers = 0;
    lock->active_writers = 0;
}

void rw_lock_acquire_read(rw_lock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    while (lock->writers > 0 || lock->active_writers > 0) {
        pthread_cond_wait(&lock->readers_cv, &lock->mutex);
    }
    lock->readers++;
    pthread_mutex_unlock(&lock->mutex);
}

void rw_lock_release_read(rw_lock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->readers--;
    if (lock->readers == 0 && lock->writers > 0) {
        pthread_cond_signal(&lock->writers_cv);
    }
    pthread_mutex_unlock(&lock->mutex);
}

void rw_lock_acquire_write(rw_lock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->writers++;
    while (lock->readers > 0 || lock->active_writers > 0) {
        pthread_cond_wait(&lock->writers_cv, &lock->mutex);
    }
    lock->writers--;
    lock->active_writers++;
    pthread_mutex_unlock(&lock->mutex);
}

void rw_lock_release_write(rw_lock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->active_writers--;
    if (lock->writers > 0) {
        pthread_cond_signal(&lock->writers_cv);
    } else {
        pthread_cond_broadcast(&lock->readers_cv);
    }
    pthread_mutex_unlock(&lock->mutex);
}
