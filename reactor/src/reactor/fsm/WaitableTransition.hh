#ifndef REACTOR_FSM_WAITABLETRANSITION_HH
# define REACTOR_FSM_WAITABLETRANSITION_HH

# include <reactor/fsm/fwd.hh>
# include <reactor/fsm/Transition.hh>

namespace reactor
{
  namespace fsm
  {
    class WaitableTransition:
      public Transition
    {
    public:
      virtual
      std::unique_ptr<Thread>
      run(reactor::Signal& triggered,
          Transition*& trigger, Thread&
          action_thread);
    protected:
      WaitableTransition(State& start,
                         State& end,
                         Waitables const& trigger,
                         bool preemptive = false);
      friend class Machine;
      ELLE_ATTRIBUTE_R(Waitables, trigger);
      ELLE_ATTRIBUTE_R(bool, preemptive);

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

#endif
