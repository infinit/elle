#include <reactor/fsm/State.hh>
#include <reactor/fsm/Transition.hh>

namespace reactor
{
  namespace fsm
  {
    Transition::Transition(State& start,
                           State& end,
                           Waitables const& trigger,
                           bool preemptive):
      _start(start),
      _end(end),
      _trigger(trigger),
      _preemptive(preemptive)
    {
      start._transitions_out.insert(this);
      end._transitions_in.insert(this);
    }
  }
}
