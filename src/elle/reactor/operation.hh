#pragma once

#include <elle/reactor/duration.hh>
#include <elle/reactor/waitable.hh>

namespace elle
{
  namespace reactor
  {
    class Operation: public Waitable
    {
    public:
      typedef Operation Self;
      typedef Waitable Super;
      Operation();
      Operation(Scheduler& scheduler);
      Scheduler& scheduler();
      bool run(DurationOpt timeout = DurationOpt());
      void start();

      ELLE_ATTRIBUTE_R(bool, running);

    protected:
      void abort();
      void done();
      virtual void _abort() = 0;
      virtual void _start() = 0;

    private:
      Scheduler& _sched;
    };
  }
}
