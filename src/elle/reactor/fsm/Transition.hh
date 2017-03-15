#pragma once

#include <boost/optional.hpp>

#include <elle/reactor/fsm/fwd.hh>
#include <elle/reactor/Waitable.hh>

namespace elle
{
  namespace reactor
  {
    namespace fsm
    {
      /// Transition between State.
      ///
      /// Transition (abstract) is an optional action to be executed between
      /// States. Transition::run is a noop.
      ///
      /// Transition can be specialized via run() and done().
      class Transition
        : public elle::Printable
      {
      public:
        using Action = std::function<void()>;
      public:
        /// Called when the parent State is over.
        ///
        /// \param triggered The Signal to trigger if we should change States.
        /// \param trigger Who trigger the action. If null, trigger should be
        ///                set to this.
        /// \param action_thread The Thread that was running the State's action.
        /// \returns An optional function to execute.
        virtual
        boost::optional<Action>
        run(reactor::Signal& triggered,
            Transition*& trigger,
            Thread& action_thread);
        /// Called at the end of the State.
        ///
        /// \param trigger The Signal to trigger if we should change States.
        /// \param exn An exception pointer to the exception responsible of
        ///            leaving the State, if any.
        virtual
        void
        done(Transition*& trigger, std::exception_ptr& exn);
      public:
        virtual
        ~Transition() = default;
      protected:
        /// Construct a Transition between two States.
        ///
        /// \param start The source State of the Transition.
        /// \param end The destination State of the Transition.
        /// \param make_first Whether the transition must be first transition
        ///                   to be executed.
        Transition(State& start,
                   State& end,
                   bool make_first = false);
        friend class Machine;
        ELLE_ATTRIBUTE_R(State&, start);
        ELLE_ATTRIBUTE_R(State&, end);
        ELLE_ATTRIBUTE_RW(Action, action);
        ELLE_ATTRIBUTE_RW(std::function<void(std::exception_ptr)>,
                          action_exception);
      protected:
        virtual
        void
        _run_action(std::exception_ptr e);
      };
    }
  }
}
