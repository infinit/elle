#include <elle/log.hh>
#include <elle/reactor/fsm/EndTransition.hh>
#include <elle/reactor/fsm/State.hh>

ELLE_LOG_COMPONENT("elle.reactor.fsm.Transition")

namespace elle
{
  namespace reactor
  {
    namespace fsm
    {
      EndTransition::EndTransition(State& start,
                                   State& end):
        Transition(start, end),
        _condition()
      {}

      EndTransition::EndTransition(State& start,
                                   State& end,
                                   std::function<bool ()> const& condition):
        Transition(start, end),
        _condition(condition)
      {}

      void
      EndTransition::done(Transition*& trigger, std::exception_ptr& exn)
      {
        if (!trigger && !exn && (!this->_condition || this->_condition.get()()))
        {
          ELLE_TRACE("%s: trigger", *this);
          trigger = this;
        }
      }

      /*----------.
      | Printable |
      `----------*/

      void
      EndTransition::print(std::ostream& stream) const
      {
        stream << "Transition(" << this->start().name() << " , "
               << this->end().name() << " , END)";
      }
    }
  }
}
