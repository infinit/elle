#pragma once

#include <elle/reactor/duration.hh>
#include <elle/reactor/Waitable.hh>

namespace elle
{
  namespace reactor
  {
    /// A background operation.
    class Operation
      : public Waitable
    {
    public:
      using Self = Operation;
      using Super = Waitable;
      Operation();
      Operation(Scheduler& scheduler);
      Scheduler&
      scheduler();
      /// Run the operation, and wait for its completion.
      ///
      /// @returns  whether it succeeded.
      bool
      run(DurationOpt timeout = DurationOpt());
      /// Start the operation.
      void
      start();

      ELLE_ATTRIBUTE_R(bool, running);

    protected:
      void
      abort();
      /// Signal completion.
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
