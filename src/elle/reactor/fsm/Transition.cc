#include <elle/log.hh>
#include <elle/reactor/fsm/State.hh>
#include <elle/reactor/fsm/Transition.hh>
#include <elle/reactor/thread.hh>

ELLE_LOG_COMPONENT("elle.reactor.fsm.Transition")

namespace elle
{
  namespace reactor
  {
    namespace fsm
    {
      boost::optional<std::function<void()>>
      Transition::run(reactor::Signal&,
                      Transition*&,
                      Thread&)
      {
        ELLE_DEBUG("Initializing %s", *this);
        return boost::optional<std::function<void()>>();
      }

      void
      Transition::done(Transition*&, std::exception_ptr&)
      {}

      Transition::Transition(State& start,
                             State& end,
                             bool make_first):
        _start(start),
        _end(end)
      {
        if (make_first)
          start._transitions_out.get<1>().insert(
            start._transitions_out.get<1>().begin(), this);
        else
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
}
