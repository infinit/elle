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
      class Transition:
        public elle::Printable
      {
      public:
        virtual
        boost::optional<std::function<void()>>
        run(reactor::Signal& triggered,
            Transition*& trigger,
            Thread& action_thread);
        virtual
        void
        done(Transition*& trigger, std::exception_ptr& exn);
      public:
        virtual
        ~Transition() = default;
      protected:
        Transition(State& start,
                   State& end,
                   bool make_first = false);
        friend class Machine;
        ELLE_ATTRIBUTE_R(State&, start);
        ELLE_ATTRIBUTE_R(State&, end);
        ELLE_ATTRIBUTE_RW(std::function<void()>, action);
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
