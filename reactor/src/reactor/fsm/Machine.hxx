#ifndef ELLE_REACTOR_MACHINE_HXX
# define ELLE_REACTOR_MACHINE_HXX

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
  }
}

#endif
