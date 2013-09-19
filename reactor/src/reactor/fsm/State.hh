#ifndef REACTOR_FSM_STATE_HH
# define REACTOR_FSM_STATE_HH

# include <functional>
# include <string>
# include <unordered_set>

# include <boost/multi_index_container.hpp>
# include <boost/multi_index/hashed_index.hpp>
# include <boost/multi_index/identity.hpp>
# include <boost/multi_index/sequenced_index.hpp>

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

    public:
      typedef boost::multi_index::multi_index_container<
        Transition*,
      boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<boost::multi_index::identity<Transition*>>,
        boost::multi_index::sequenced<>
        >
      > Transitions;

      ELLE_ATTRIBUTE_R(Transitions, transitions_out);
      ELLE_ATTRIBUTE_R(Transitions, transitions_in);

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
