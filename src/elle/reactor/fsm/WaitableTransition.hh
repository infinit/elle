#pragma once

#include <elle/reactor/fsm/fwd.hh>
#include <elle/reactor/fsm/Transition.hh>

namespace elle
{
  namespace reactor
  {
    namespace fsm
    {
      class WaitableTransition:
        public Transition
      {
      public:
        using PreTrigger = std::function<bool ()>;
        boost::optional<std::function<void()>>
        run(reactor::Signal& triggered,
            Transition*& trigger,
            Thread& action_thread) override;
      protected:
        WaitableTransition(State& start,
                           State& end,
                           Waitables const& trigger,
                           bool preemptive = false,
                           PreTrigger const& pre_trigger = PreTrigger());
        friend class Machine;
        ELLE_ATTRIBUTE_R(Waitables, trigger);
        ELLE_ATTRIBUTE_R(bool, preemptive);
        ELLE_ATTRIBUTE_R(std::function<bool ()>, pre_trigger);

      /*----------.
      | Printable |
      `----------*/
      public:
        void
        print(std::ostream& stream) const override;
      };
    }
  }
}
