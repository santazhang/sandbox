#ifndef PTHREAD_MACOSX_HACK_H_
#define PTHREAD_MACOSX_HACK_H_ 1

#include <pthread.h>

#ifdef __APPLE__

#include <errno.h>

#define THRIFT_MUTEX_EMULATE_PTHREAD_TIMEDLOCK

static int pthread_getattr_np(pthread_t thread, pthread_attr_t *attr) {
    return ENOSYS;
}

#endif  // __APPLE__

#endif  // PTHREAD_MACOSX_HACK_H_
