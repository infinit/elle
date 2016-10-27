#ifndef INFINIT_REACTOR_LOCKABLE_HH
# define INFINIT_REACTOR_LOCKABLE_HH

# include <reactor/waitable.hh>

namespace reactor
{
  class Lockable: public Waitable
  {
    public:
      /// Try to acquire the lock, without blocking
      virtual bool acquire() = 0;
      virtual bool release() = 0;
  };

  class Lock
  {
    public:
      Lock(Lockable& lockable);
      ~Lock();

    private:
      Lockable& _lockable;
  };
}

#endif
