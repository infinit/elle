#include <reactor/fsm/CatchTransition.hh>

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
        std::make_unique<State>(std::forward<Args>(args)...));
    }

    /*------------.
    | Transitions |
    `------------*/

    template <typename T>
    Transition&
    Machine::transition_add_catch_specific(State& start,
                                           State& end,
                                           bool front)
    {
      auto transition =
        std::unique_ptr<CatchTransition>
        (new CatchTransition(start, end, front));
      auto condition = std::function<bool (std::exception_ptr const&)>{
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
        }};
      transition->condition(condition);
      Transition& res = *transition.get();
      this->_transitions.insert(std::move(transition));
      return res;
    }
  }
}
