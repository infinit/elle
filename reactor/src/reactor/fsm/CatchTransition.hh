#ifndef REACTOR_FSM_CATCHTRANSITION_HH
# define REACTOR_FSM_CATCHTRANSITION_HH

# include <reactor/fsm/Transition.hh>
# include <reactor/fsm/fwd.hh>

namespace reactor
{
  namespace fsm
  {
    class CatchTransition:
      public Transition
    {
    public:
      virtual
      void
      done(Transition*& trigger, std::exception_ptr& exn);
      ELLE_ATTRIBUTE_RW(std::function<bool (std::exception_ptr const&)>,
                        condition);
    protected:
      CatchTransition(State& start,
                      State& end);
      friend class Machine;
    };
  }
}

#endif
