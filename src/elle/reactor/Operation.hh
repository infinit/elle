#pragma once

#include <elle/reactor/duration.hh>
#include <elle/reactor/Waitable.hh>

namespace elle
{
  namespace reactor
  {
    /// Operation is an abstract type of Waitable to provide the notion of
    /// operation to perform. A Thread waiting on an operation will be awaken
    /// when the Operation is done.
    ///
    /// Operation::run will wait until the operation completes.
    ///
    /// Class inheriting from Operation must implement _abort and _start.
    /// - _start should start the actual operation.
    /// - _abort should cancel the operation.
    class Operation
      : public Waitable
    {
    public:
      using Self = Operation;
      using Super = Waitable;
      /// Create a operation for the current Scheduler.
      Operation();
      /// Create a operation for the given Scheduler.
      ///
      /// \param scheduler The Scheduler to run the Operation.
      Operation(Scheduler& scheduler);
      /// Return the
      /// Run the operation (by invoking _start()) and wait until it
      /// completes.
      ///
      /// \param timeout The maximum duration the Operation has to perform.
      /// \returns True if the operation complete, false if it timed out.
      bool
      run(DurationOpt timeout = DurationOpt());
      /// Set _running to true and start the operation by running
      /// _start().
      void
      start();
      /// Wait for completion.
      ///
      /// \param   timeout Time to wait before giving up.
      /// \returns whether It did not time out.
      bool
      join(DurationOpt timeout = DurationOpt());

      ELLE_ATTRIBUTE_R(bool, running);

    protected:
      /// Abort the Operation if it's running by calling _abort.
      void
      abort();
      /// Set _running to false and call Waitable::_signal().
      void
      done();
      /// Start the actual Operation.
      virtual
      void
      _start() = 0;
      /// Cancel the Operation.
      virtual
      void
      _abort() = 0;

    private:
      ELLE_ATTRIBUTE_RX(Scheduler&, sched);
    };
  }
}
