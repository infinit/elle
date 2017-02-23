#include <elle/log.hh>
#include <elle/reactor/mutex.hh>
#include <elle/reactor/pthread.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/signal.hh>

ELLE_LOG_COMPONENT("elle.reactor.pthread");

extern "C"
void
reactor_pthread_mutex_lock(reactor_pthread_mutex_t *m)
{
  if (!*m)
    *m = new elle::reactor::Mutex();
  ELLE_TRACE("reactor_pthread_mutex_lock %s", *m);
  elle::reactor::wait(**m);
  ELLE_TRACE("reactor_pthread_mutex_lock exit %s", *m);
}

extern "C"
void
reactor_pthread_mutex_unlock(reactor_pthread_mutex_t *m)
{
  ELLE_TRACE("reactor_pthread_mutex_unlock %s", *m);
  if (!*m)
    *m = new elle::reactor::Mutex();
  (*m)->release();
}

extern "C"
void
reactor_pthread_mutex_destroy(reactor_pthread_mutex_t *m)
{
  ELLE_TRACE("reactor_pthread_mutex_destroy %s", *m);
  if (*m)
    delete *m;
  *m = nullptr;
}


extern "C"
void
reactor_pthread_cond_init(reactor_pthread_cond_t *c, void*)
{
  *c = new elle::reactor::Signal();
}

extern "C"
void
reactor_pthread_cond_signal(reactor_pthread_cond_t *c)
{
  (*c)->signal_one();
}

extern "C"
void
reactor_pthread_cond_broadcast(reactor_pthread_cond_t *c)
{
  (*c)->signal();
}

extern "C"
void
reactor_pthread_cond_destroy(reactor_pthread_cond_t * c)
{
  delete *c;
  *c = nullptr;
}

extern "C"
void
reactor_pthread_cond_wait(reactor_pthread_cond_t * c, reactor_pthread_mutex_t * m)
{
  ELLE_TRACE("reactor_pthread_cond_wait %s", *m);
  (*m)->release();
  elle::reactor::wait(**c);
  elle::reactor::wait(**m);
}

extern "C"
void
reactor_pthread_cond_timedwait(reactor_pthread_cond_t * c,
                               reactor_pthread_mutex_t * m,
                               struct timespec* timeout)
{
  ELLE_TRACE("reactor_pthread_cond_timedwait %s", *m);
  struct timeval now;
  gettimeofday(&now, NULL);
  int msDelta = (timeout->tv_sec - now.tv_sec) * 1000;
  msDelta += timeout->tv_nsec/1000000 - now.tv_usec / 1000;
  (*m)->release();
  try {
    elle::reactor::wait(**c, boost::posix_time::millisec(msDelta));
  }
  catch(...) {}
  elle::reactor::wait(**m);
}


extern "C"
void
reactor_mixed_pthread_cond_wait(reactor_pthread_cond_t * c, pthread_mutex_t * m)
{
  ELLE_TRACE("reactor_pthread_cond_wait %s", *m);
  pthread_mutex_unlock(m);
  elle::reactor::wait(**c);
  pthread_mutex_lock(m);
}

extern "C"
void
reactor_mixed_pthread_cond_timedwait(reactor_pthread_cond_t * c,
                                     pthread_mutex_t * m,
                                     struct timespec* timeout)
{
  ELLE_TRACE("reactor_pthread_cond_timedwait %s", *m);
  struct timeval now;
  gettimeofday(&now, NULL);
  int msDelta = (timeout->tv_sec - now.tv_sec) * 1000;
  msDelta += timeout->tv_nsec/1000000 - now.tv_usec / 1000;
  pthread_mutex_unlock(m);
  try {
    elle::reactor::wait(**c, boost::posix_time::millisec(msDelta));
  }
  catch(...) {}
  pthread_mutex_lock(m);
}
