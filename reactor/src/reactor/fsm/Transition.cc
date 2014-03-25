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

    void
    Transition::_run_action(std::exception_ptr e)
    {
      if (this->action())
        this->action()();
      if (this->action_exception())
        this->action_exception()(e);
    }
  }
}
