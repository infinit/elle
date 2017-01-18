#pragma once

#include <reactor/fsm/fwd.hh>
#include <reactor/fsm/Transition.hh>

namespace reactor
{
  namespace fsm
  {
    class WaitableTransition:
      public Transition
    {
    public:
      typedef std::function<bool ()> PreTrigger;
      virtual
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
      virtual
      void
      print(std::ostream& stream) const override;
    };
  }
}
