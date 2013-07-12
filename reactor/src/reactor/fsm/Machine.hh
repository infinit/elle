#ifndef ELLE_FSM_MACHINE_HH
# define ELLE_FSM_MACHINE_HH

# include <memory>
# include <unordered_set>
# include <vector>

# include <reactor/fsm/State.hh>
# include <reactor/fsm/Transition.hh>

namespace reactor
{
  namespace fsm
  {
    class Machine
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      Machine();

    /*-------.
    | States |
    `-------*/
    public:
      template <typename ... Args>
      State&
      state_make(Args&& ...);
      State&
      state_add(std::unique_ptr<State> state);
    private:
      ELLE_ATTRIBUTE(std::unordered_set<std::unique_ptr<State>>, states);
      ELLE_ATTRIBUTE_R(State*, start);

    /*------------.
    | Transitions |
    `------------*/
    public:
      Transition&
      transition_add(State& start,
                     State& end,
                     Waitables const& trigger = Waitables(),
                     bool preemptive = false);
    private:
      ELLE_ATTRIBUTE(
        std::unordered_set<std::unique_ptr<Transition>>, transitions);

    /*----------.
    | Execution |
    `----------*/
    public:
      void
      run();
    private:
      State*
      _run_state(State* state);
      ELLE_ATTRIBUTE_R(bool, running);
    };
  }
}

# include <reactor/fsm/Machine.hxx>

#endif
