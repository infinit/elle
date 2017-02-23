#pragma once

#include <pthread.h>
#include <elle/reactor/mutex.hh>
#include <elle/reactor/signal.hh>

typedef elle::reactor::Mutex* reactor_pthread_mutex_t;
typedef elle::reactor::Signal* reactor_pthread_cond_t;

// Provide a pthread API with reactor as a backend
extern "C" void reactor_pthread_mutex_lock(reactor_pthread_mutex_t *m);
extern "C" void reactor_pthread_mutex_unlock(reactor_pthread_mutex_t *m);
extern "C" void reactor_pthread_mutex_destroy(reactor_pthread_mutex_t *m);
extern "C" void reactor_pthread_cond_init(reactor_pthread_cond_t *c, void*);
extern "C" void reactor_pthread_cond_signal(reactor_pthread_cond_t *c);
extern "C" void reactor_pthread_cond_broadcast(reactor_pthread_cond_t *c);
extern "C" void reactor_pthread_cond_destroy(reactor_pthread_cond_t * c);
extern "C" void reactor_pthread_cond_wait(reactor_pthread_cond_t * c,
                                          reactor_pthread_mutex_t * m);
extern "C" void reactor_pthread_cond_timedwait(reactor_pthread_cond_t * c,
                                               reactor_pthread_mutex_t * m,
                                               struct timespec* timeout);
extern "C" void reactor_mixed_pthread_cond_wait(reactor_pthread_cond_t * c,
                                                pthread_mutex_t * m);
extern "C" void reactor_mixed_pthread_cond_timedwait(reactor_pthread_cond_t * c,
                                                     pthread_mutex_t * m,
                                                     struct timespec* timeout);
