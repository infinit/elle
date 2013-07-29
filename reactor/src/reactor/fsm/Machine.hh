#ifndef ELLE_FSM_MACHINE_HH
# define ELLE_FSM_MACHINE_HH

# include <memory>
# include <unordered_set>
# include <vector>

# include <elle/Printable.hh>

# include <reactor/fsm/State.hh>
# include <reactor/fsm/Transition.hh>

namespace reactor
{
  namespace fsm
  {
    class Machine:
      public elle::Printable
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef std::function<bool ()> PreTrigger;

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
      transition_add(
        State& start,
        State& end,
        Waitables const& trigger,
        bool preemptive = false,
        PreTrigger const& pre_trigger = PreTrigger());
      Transition&
      transition_add(State& start,
                     State& end);
      Transition&
      transition_add_catch(State& start,
                           State& end);
    private:
      ELLE_ATTRIBUTE(
        std::unordered_set<std::unique_ptr<Transition>>, transitions);

    /*----------.
    | Execution |
    `----------*/
    public:
      void
      run();
      void
      run(State& start);
    private:
      State*
      _run_state(State* state);
      ELLE_ATTRIBUTE_R(bool, running);
      ELLE_ATTRIBUTE_R(std::exception_ptr, exception);

    /*----------.
    | Printable |
    `----------*/
    public:
      virtual
      void
      print(std::ostream& stream) const;
    };
  }
}

# include <reactor/fsm/Machine.hxx>

#endif
