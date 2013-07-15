#include <elle/log.hh>

#include <reactor/fsm/CatchTransition.hh>

ELLE_LOG_COMPONENT("reactor.fsm.Transition")

namespace reactor
{
  namespace fsm
  {
    CatchTransition::CatchTransition(State& start,
                                     State& end):
      Transition(start, end)
    {}

    void
    CatchTransition::done(Transition*& trigger, std::exception_ptr& exn)
    {
      if (exn)
      {
        ELLE_TRACE("%s: trigger", *this);
        trigger = this;
        exn = std::exception_ptr();
      }
    }
  }
}
