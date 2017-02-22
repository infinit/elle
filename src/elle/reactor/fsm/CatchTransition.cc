#include <elle/log.hh>
#include <elle/reactor/fsm/CatchTransition.hh>
#include <elle/reactor/fsm/State.hh>

ELLE_LOG_COMPONENT("elle.reactor.fsm.Transition")

namespace elle
{
  namespace reactor
  {
    namespace fsm
    {
      CatchTransition::CatchTransition(State& start,
                                       State& end,
                                       bool make_first)
        : Transition(start, end, make_first)
      {}

      void
      CatchTransition::done(Transition*& trigger, std::exception_ptr& exn)
      {
        if (exn)
          if (!this->condition() || this->condition()(exn))
          {
            ELLE_TRACE("%s: trigger: %s", *this, elle::exception_string(exn));
            trigger = this;
            exn = std::exception_ptr();
          }
      }

      /*----------.
      | Printable |
      `----------*/

      void
      CatchTransition::print(std::ostream& stream) const
      {
        stream << "Transition(" << this->start().name() << " , "
               << this->end().name() << " , EXCEPTION)";
      }
    }
  }
}
