#pragma once

#include <elle/optional.hh>

#include <elle/reactor/Operation.hh>

namespace elle
{
  namespace reactor
  {
    namespace
    {
      template <typename T>
      class BackgroundOperationResult
      {
      protected:
        void
        _result_set(T&& v);
        ELLE_ATTRIBUTE_R(boost::optional<T>, result, protected);
      };

      template <>
      class BackgroundOperationResult<void>
      {
      protected:
        void
        _result_set(bool);
      };
    }

    /// BackgroundOperation is a specialized Operation to run background
    /// operations in the Scheduler.
    ///
    /// BackgroundOperation run the action in an std::thread and handle the
    /// locking for you.
    template<typename T>
    class BackgroundOperation
      : public Operation
      , public BackgroundOperationResult<T>
    {
    public:
      using Action = std::function<T ()>;
      struct Status
      {
        bool aborted;
      };

    public:
      /// Construct a BackgroundOperation from an Action.
      ///
      /// \param action The Action to perform.
      BackgroundOperation(Action const& action);
      ~BackgroundOperation();
      ELLE_ATTRIBUTE(Action, action);
      ELLE_ATTRIBUTE(std::shared_ptr<Status>, status);

    protected:
      /// Start the Operation by running the Action via
      /// Scheduler::_run_background.
      ///
      /// When the operation is over, store the in
      /// BackgroundOperationResult::_result and call Operation::done();
      void
      _start() override;
      /// Abort the current BackgroundOperation, ignoring its result.
      ///
      /// Operation::done() is called but BackgroundOperationResult::_result is
      /// is left empty.
      void
      _abort() override;
    };
  }
}

#include <elle/reactor/BackgroundOperation.hxx>
