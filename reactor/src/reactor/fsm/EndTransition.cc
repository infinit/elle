#include <elle/log.hh>

#include <reactor/fsm/EndTransition.hh>
#include <reactor/fsm/State.hh>

ELLE_LOG_COMPONENT("reactor.fsm.Transition")

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
      stream << "transition on state end "
             << "from " << this->start() << " to " << this->end();
    }
  }
}
