#include <reactor/fsm/State.hh>
#include <reactor/fsm/Transition.hh>
#include <reactor/thread.hh>

namespace reactor
{
  namespace fsm
  {
    std::unique_ptr<Thread>
    Transition::run(reactor::Signal&,
                    Transition*&,
                    Thread&)
    {
      return nullptr;
    }

    void
    Transition::done(Transition*&, std::exception_ptr&)
    {}

    Transition::Transition(State& start,
                           State& end):
      _start(start),
      _end(end)
    {
      start._transitions_out.insert(this);
      end._transitions_in.insert(this);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Transition::print(std::ostream& stream) const
    {
      stream << "transition " << this;
    }
  }
}
