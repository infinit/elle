#ifndef REACTOR_FSM_TRANSITION_HH
# define REACTOR_FSM_TRANSITION_HH

# include <boost/optional.hpp>

# include <reactor/fsm/fwd.hh>
# include <reactor/waitable.hh>

namespace reactor
{
  namespace fsm
  {
    class Transition:
      public elle::Printable
    {
    public:
      virtual
      boost::optional<std::function<void()>>
      run(reactor::Signal& triggered,
          Transition*& trigger,
          Thread& action_thread);
      virtual
      void
      done(Transition*& trigger, std::exception_ptr& exn);
    public:
      virtual
      ~Transition() = default;
    protected:
      Transition(State& start,
                 State& end);
      friend class Machine;
      ELLE_ATTRIBUTE_R(State&, start);
      ELLE_ATTRIBUTE_R(State&, end);
      ELLE_ATTRIBUTE_RW(std::function<void()>, action);
    };
  }
}

#endif
