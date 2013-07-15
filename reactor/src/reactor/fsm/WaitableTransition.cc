#include <elle/log.hh>
#include <elle/memory.hh>

#include <reactor/fsm/WaitableTransition.hh>
#include <reactor/scheduler.hh>
#include <reactor/signal.hh>

ELLE_LOG_COMPONENT("reactor.fsm.Transition")

namespace reactor
{
  namespace fsm
  {
    std::unique_ptr<Thread>
    WaitableTransition::run(reactor::Signal& triggered,
                            Transition*& trigger,
                            Thread& action_thread)
    {
      ELLE_ASSERT(Scheduler::scheduler());
      auto& sched = *Scheduler::scheduler();

      ELLE_DEBUG("%s: start on %s", *this, this->trigger());
      return elle::make_unique<Thread>(
        sched,
        elle::sprint(*this),
        [this,
         &sched, &trigger, &action_thread, &triggered](){
          sched.current()->wait(this->trigger());
          if (!trigger)
          {
            ELLE_TRACE("%s: trigger", *this);
            trigger = this;
            if (this->preemptive())
              action_thread.terminate();
            triggered.signal();
          }
        });
    }

    WaitableTransition::WaitableTransition(State& start,
                                           State& end,
                                           Waitables const& trigger,
                                           bool preemptive):
      Transition(start, end),
      _trigger(trigger),
      _preemptive(preemptive)
    {}
  }
}
