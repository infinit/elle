#include <elle/log.hh>
#include <elle/memory.hh>
#include <elle/reactor/fsm/State.hh>
#include <elle/reactor/fsm/WaitableTransition.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/signal.hh>
#include <utility>

ELLE_LOG_COMPONENT("elle.reactor.fsm.Transition")

namespace elle
{
  namespace reactor
  {
    namespace fsm
    {
      boost::optional<std::function<void()>>
      WaitableTransition::run(reactor::Signal& triggered,
                              Transition*& trigger,
                              Thread& action_thread)
      {
        ELLE_ASSERT(Scheduler::scheduler());
        auto& sched = *Scheduler::scheduler();

        if (this->_pre_trigger && this->_pre_trigger())
        {
          ELLE_DEBUG("%s: pre-triggered", *this);
          if (!trigger)
          {
            ELLE_TRACE("%s: trigger", *this);
            trigger = this;
            // FIXME: thread isn't started yet.
            if (this->preemptive())
              action_thread.terminate();
            triggered.signal();
          }
          return boost::none;
        }

        ELLE_DEBUG("Initializing %s", *this);
        return std::function<void ()>(
          [this,
           &sched,
           &trigger,
           &action_thread,
           &triggered] ()
          {
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
      };

      WaitableTransition::WaitableTransition(State& start,
                                             State& end,
                                             Waitables  trigger,
                                             bool preemptive,
                                             PreTrigger  pre_trigger):
        Transition(start, end),
        _trigger(std::move(trigger)),
        _preemptive(preemptive),
        _pre_trigger(std::move(pre_trigger))
      {}

      /*----------.
      | Printable |
      `----------*/

      void
      WaitableTransition::print(std::ostream& stream) const
      {
        stream << "Transition(" << this->start().name() << " , "
               << this->end().name() << " , " << this->trigger() << ')';
      }
    }
  }
}
