#ifndef REACTOR_FSM_STATE_HH
# define REACTOR_FSM_STATE_HH

# include <functional>
# include <string>
# include <unordered_set>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <reactor/fsm/fwd.hh>
# include <reactor/signal.hh>

namespace reactor
{
  namespace fsm
  {
    class State:
      public elle::Printable
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef std::function<void ()> Action;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      explicit
      State(Action const& action);
      explicit
      State(char const* name);
      explicit
      State(std::string const& name = "", Action const& action = [] () {});
    private:
      friend class Machine;
      ELLE_ATTRIBUTE(Action, action);
      ELLE_ATTRIBUTE_R(std::string, name);

    /*------------.
    | Transitions |
    `------------*/
    private:
      friend class Transition;
      ELLE_ATTRIBUTE_R(std::unordered_set<Transition*>, transitions_out);
      ELLE_ATTRIBUTE_R(std::unordered_set<Transition*>, transitions_in);

    /*----------.
    | Execution |
    `----------*/
    private:
      ELLE_ATTRIBUTE_RX(reactor::Signal, done);
      ELLE_ATTRIBUTE_RX(reactor::Signal, entered);
      ELLE_ATTRIBUTE_RX(reactor::Signal, exited);

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
