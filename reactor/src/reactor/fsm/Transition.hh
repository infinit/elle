#ifndef REACTOR_FSM_TRANSITION_HH
# define REACTOR_FSM_TRANSITION_HH

# include <reactor/fsm/fwd.hh>
# include <reactor/waitable.hh>

namespace reactor
{
  namespace fsm
  {
    class Transition
    {
    private:
      Transition(State& start,
                 State& end,
                 Waitables const& trigger,
                 bool preemptive = false);
      friend class Machine;
      ELLE_ATTRIBUTE_R(State&, start);
      ELLE_ATTRIBUTE_R(State&, end);
      ELLE_ATTRIBUTE_R(Waitables, trigger);
      ELLE_ATTRIBUTE_R(bool, preemptive);
    };
  }
}

#endif
