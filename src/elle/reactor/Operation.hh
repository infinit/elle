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
      /// Start the operation.
      void
      start();
      /// Wait for completion.
      ///
      /// @param   timeout Time to wait before giving up.
      /// @returns whether It did not time out.
      bool
      join(DurationOpt timeout = DurationOpt());
      /// Start the operation and wait for its completion.
      ///
      /// @param   timeout Time to wait before giving up.
      /// @returns  whether it did not time out.
      bool
      run(DurationOpt timeout = DurationOpt());

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
