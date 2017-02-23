#pragma once

#include <elle/reactor/lockable.hh>

namespace elle
{
  namespace reactor
  {
    class Semaphore
      : public Lockable
    {
    public:
      Semaphore(int count = 0);
      int count() const;
      bool acquire() override;
      bool release() override;

    protected:
      virtual
      bool
      _wait(Thread* thread, Waker const& waker) override;

    private:
      int _count;
    };
  }
}
