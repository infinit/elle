#ifndef INFINIT_REACTOR_LOCKABLE_HH
# define INFINIT_REACTOR_LOCKABLE_HH

# include <reactor/scheduler.hh>
# include <reactor/waitable.hh>

namespace reactor
{
  class Lockable: public Waitable
  {
    public:
      virtual bool release() = 0;
  };

  class Lock
  {
    public:
      Lock(Scheduler& sched, Lockable& lockable);
      ~Lock();

    private:
      Lockable& _lockable;
  };
}

#endif
