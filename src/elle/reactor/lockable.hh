#pragma once

#include <elle/reactor/Waitable.hh>

namespace elle
{
  namespace reactor
  {
    class Lockable
      : public Waitable
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
}
