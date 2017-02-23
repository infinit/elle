#pragma once

#include <elle/reactor/lockable.hh>

namespace elle
{
  namespace reactor
  {
    class Mutex
      : public Lockable
    {
    public:
      Mutex();
      virtual
      ~Mutex();
      Mutex(Mutex&&) = default;
      bool locked() const;
      bool release() override;
      bool acquire() override;
    protected:
      bool _wait(Thread* thread, Waker const& waker) override;

    private:
      bool _locked;
    };
  }
}
