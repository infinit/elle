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
      EndTransition(State& start,
                    State& end,
                    std::function<bool ()> const& condition);
      friend class Machine;
      ELLE_ATTRIBUTE(boost::optional<std::function<bool ()>>, condition);

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
