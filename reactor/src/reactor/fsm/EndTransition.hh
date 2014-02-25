#ifndef REACTOR_FSM_ENDTRANSITION_HH
# define REACTOR_FSM_ENDTRANSITION_HH

# include <reactor/fsm/Transition.hh>
# include <reactor/fsm/fwd.hh>

namespace reactor
{
  namespace fsm
  {
    class EndTransition:
      public Transition
    {
    public:
      virtual
      void
      done(Transition*& trigger, std::exception_ptr& exn);
    protected:
      EndTransition(State& start,
                    State& end);
      friend class Machine;

    /*----------.
    | Printable |
    `----------*/
    public:
      virtual
      void
      print(std::ostream& stream) const override;
    };
  }
}

#endif
