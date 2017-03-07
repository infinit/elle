#include <elle/With.hh>
#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/memory.hh>
#include <elle/reactor/Scope.hh>
#include <elle/reactor/exception.hh>
#include <elle/reactor/fsm/CatchTransition.hh>
#include <elle/reactor/fsm/EndTransition.hh>
#include <elle/reactor/fsm/Machine.hh>
#include <elle/reactor/fsm/WaitableTransition.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/signal.hh>
#include <utility>

ELLE_LOG_COMPONENT("elle.reactor.fsm.Machine")

namespace elle
{
  namespace reactor
  {
    namespace fsm
    {
      /*-------------.
      | Construction |
      `-------------*/

      Machine::Machine(std::string  name):
        _states(),
        _start(nullptr),
        _transitions(),
        _running(false),
        _exception(nullptr),
        _current_state(nullptr),
        _name(std::move(name))
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
                              bool preemptive,
                              std::function<bool ()> const& pre_trigger)
      {
        std::unique_ptr<Transition> transition(
          new WaitableTransition(start, end, trigger, preemptive, pre_trigger));
        Transition& res = *transition.get();
        this->_transitions.insert(std::move(transition));
        return res;
      }

      Transition&
      Machine::transition_add(State& start,
                              State& end)
      {
        std::unique_ptr<Transition> transition(
          new EndTransition(start, end));
        Transition& res = *transition.get();
        this->_transitions.insert(std::move(transition));
        return res;
      }

      Transition&
      Machine::transition_add(State& start,
                              State& end,
                              std::function<bool ()> const& condition)
      {
        std::unique_ptr<Transition> transition(
          new EndTransition(start, end, condition));
        Transition& res = *transition.get();
        this->_transitions.insert(std::move(transition));
        return res;
      }

      Transition&
      Machine::transition_add_catch(State& start,
                                    State& end)
      {
        std::unique_ptr<Transition> transition(
          new CatchTransition(start, end));
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
        run(*this->_start);
      }

      void
      Machine::run(State& start)
      {
        ELLE_TRACE_SCOPE("%s: run", *this);
        this->_running = true;
        elle::SafeFinally finally([&] { this->_running = false; });
        this->_current_state = &start;
        while (this->_current_state = this->_run_state(this->_current_state))
          /* nothing */;
      }

      State*
      Machine::_run_state(State* state)
      {
        ELLE_TRACE_SCOPE("%s: run %s", *this, *state);
        this->_state_changed(*state);
        auto& sched = reactor::scheduler();
        auto action = [&] () {
          try
          {
            // XXX: ensures transitions were registered.
            sched.current()->yield();
            state->_action();
          }
          catch (reactor::Terminate const&)
          {
            throw;
          }
          catch (std::exception const&)
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
        return elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
        {
          for (auto transition: state->transitions_out().get<1>())
            if (auto t = transition->run(triggered, trigger, action_thread))
              scope.run_background(
                elle::sprintf("%s %s", *this, *transition), t.get());
          state->_entered.signal();
          elle::SafeFinally exited([&]() { state->_exited.signal(); });
          try
          {
            sched.current()->wait(action_thread);
          }
          catch (reactor::Terminate&)
          {
            auto e = std::current_exception();
            action_thread.terminate_now();
            std::rethrow_exception(e);
          }
          state->_done.signal();
          ELLE_DEBUG("%s: %s action finished", *this, *state);
          while (true)
          {
            // Ordered by insertion order.
            auto saved_exn = this->_exception;
            for (auto transition: state->transitions_out().get<1>())
              transition->done(trigger, this->_exception);
            if (this->_exception)
            {
              ELLE_WARN("%s: state action threw: %s",
                        *this, elle::exception_string(this->_exception));
              std::rethrow_exception(this->_exception);
            }
            if (state->transitions_out().empty())
            {
              ELLE_DEBUG("%s: end state, leaving", *this);
              return (State*)(nullptr);
            }
            while (!trigger)
            {
              ELLE_DEBUG("%s: waiting for transition", *this);
              sched.current()->wait(triggered);
            }
            this->_transition_triggered(*trigger);
            try
            {
              trigger->_run_action(saved_exn);
            }
            catch (...)
            {
              this->_exception = std::current_exception();
              continue;
            }
            return &trigger->end();
          }
        };
      }

      /*----------.
      | Printable |
      `----------*/

      void
      Machine::print(std::ostream& stream) const
      {
        if (!this->_name.empty())
          stream << this->_name;
        else
          stream << "FSM " << this;
      }
    }
  }
}
