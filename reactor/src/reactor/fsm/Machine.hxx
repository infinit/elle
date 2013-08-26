#ifndef ELLE_REACTOR_MACHINE_HXX
# define ELLE_REACTOR_MACHINE_HXX

# include <reactor/fsm/CatchTransition.hh>

namespace reactor
{
  namespace fsm
  {
    /*-------.
    | States |
    `-------*/

    template <typename ... Args>
    State&
    Machine::state_make(Args&& ... args)
    {
      return this->state_add(
        std::unique_ptr<State>(new State(std::forward<Args>(args) ...)));
    }

    /*------------.
    | Transitions |
    `------------*/

    template <typename T>
    Transition&
    Machine::transition_add_catch_specific(State& start,
                                           State& end)
    {
      std::unique_ptr<CatchTransition> transition(
        new CatchTransition(start, end));
      std::function<bool (std::exception_ptr const&)> condition =
        [] (std::exception_ptr const& exn) -> bool
        {
          try
          {
            std::rethrow_exception(exn);
          }
          catch (T const&)
          {
            return true;
          }
          catch (...)
          {
            return false;
          }
        };
      transition->condition(condition);
      Transition& res = *transition.get();
      this->_transitions.insert(std::move(transition));
      return res;
    }

  }
}

#endif
