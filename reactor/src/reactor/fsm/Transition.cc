#include <reactor/fsm/State.hh>
#include <reactor/fsm/Transition.hh>
#include <reactor/thread.hh>

namespace reactor
{
  namespace fsm
  {
    boost::optional<std::function<void()>>
    Transition::run(reactor::Signal&,
                    Transition*&,
                    Thread&)
    {
      return boost::optional<std::function<void()>>();
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
  }
}
