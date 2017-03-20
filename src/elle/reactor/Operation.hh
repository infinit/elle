#pragma once

#include <elle/reactor/duration.hh>
#include <elle/reactor/Waitable.hh>

namespace elle
{
  namespace reactor
  {
    class Operation
      : public Waitable
    {
    public:
      using Self = elle::reactor::Operation;
      using Super = elle::reactor::Waitable;
      Operation();
      Operation(Scheduler& scheduler);
      Scheduler&
      scheduler();
      bool
      run(DurationOpt timeout = DurationOpt());
      void
      start();

      ELLE_ATTRIBUTE_R(bool, running);

    protected:
      void
      abort();
      void
      done();
      virtual
      void
      _abort() = 0;
      virtual
      void
      _start() = 0;

    private:
      Scheduler& _sched;
    };
  }
}
