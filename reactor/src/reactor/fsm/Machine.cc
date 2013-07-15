#include <elle/finally.hh>
#include <elle/log.hh>

#include <reactor/fsm/Machine.hh>
#include <reactor/scheduler.hh>
#include <reactor/signal.hh>

ELLE_LOG_COMPONENT("reactor.fsm.Machine")

namespace reactor
{
  namespace fsm
  {
    /*-------------.
    | Construction |
    `-------------*/

    Machine::Machine():
      _states(),
      _start(nullptr)
    {}

    /*-------.
    | States |
    `-------*/

    State&
    Machine::state_add(std::unique_ptr<State> state)
    {
      bool first = this->_states.empty();
      State* naked = state.get();
      this->_states.insert(std::move(state));
      if (first)
        this->_start = naked;
      return *naked;
    }

    /*------------.
    | Transitions |
    `------------*/

    Transition&
    Machine::transition_add(State& start,
                            State& end,
                            Waitables const& trigger,
                            bool preemptive)
    {
      std::unique_ptr<Transition> transition(
        new Transition(start, end, trigger, preemptive));
      Transition& res = *transition.get();
      this->_transitions.insert(std::move(transition));
      return res;
    }

    /*----------.
    | Execution |
    `----------*/

    void
    Machine::run()
    {
      ELLE_TRACE_SCOPE("%s: run", *this);
      this->_running = true;
      State* current = this->_start;
      while (current = this->_run_state(current))
        /* nothing */;
      this->_running = false;
    }

    State*
    Machine::_run_state(State* state)
    {
      ELLE_TRACE_SCOPE("%s: run state %s", *this, *state);
      ELLE_ASSERT(Scheduler::scheduler());
      auto& sched = *Scheduler::scheduler();
      auto action = [&] () {
        try
        {
          state->_action();
        }
        catch (elle::Exception&)
        {
          ELLE_DEBUG("%s: forward exception: %s",
                     *this, elle::exception_string());
          this->_exception = std::current_exception();
          return;
        };
      };
      Thread action_thread(sched, elle::sprintf("%s action", *state), action);
      reactor::Signal triggered;
      Transition* trigger(nullptr);
      std::vector<std::unique_ptr<Thread>> transitions;
      for (auto transition: state->transitions_out())
      {
        if (transition->trigger().empty())
        {
          ELLE_ASSERT(!trigger);
          trigger = transition;
        }
        else
        {
          ELLE_DEBUG("%s: start transition %s on %s",
                     *this, *transition, transition->trigger());
          transitions.emplace_back(
            new Thread(sched,
                       elle::sprintf("%s transition %s", state, *transition),
                       [transition, this,
                        &sched, &trigger, &action_thread, &triggered](){
                         sched.current()->wait(transition->trigger());
                         if (!trigger)
                         {
                           ELLE_TRACE("%s: trigger transition %s",
                                      *this, *transition);
                           trigger = transition;
                           if (transition->preemptive())
                             action_thread.terminate();
                           triggered.signal();
                         }
                       }));
        }
      }
      state->_entered.signal();
      elle::Finally exited([&]() {state->_exited.signal(); });
      sched.current()->wait(action_thread);
      state->_done.signal();
      ELLE_DEBUG("%s: state action finished", *this);
      if (this->_exception)
      {
        ELLE_WARN("%s: state action threw: %s",
                  *this, elle::exception_string(this->_exception));
        std::rethrow_exception(this->_exception);
      }
      if (state->transitions_out().empty())
      {
        ELLE_DEBUG("%s: end state, leaving", *this);
        return nullptr;
      }
      while (!trigger)
      {
        ELLE_DEBUG("%s: waiting for transition", *this);
        sched.current()->wait(triggered);
      }
      for (auto& thread: transitions)
        thread->terminate_now();
      return &trigger->end();
    }
  }
}
