#pragma once

#include <functional>
#include <string>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/sequenced_index.hpp>

#include <elle/Printable.hh>
#include <elle/attribute.hh>
#include <elle/reactor/fsm/fwd.hh>
#include <elle/reactor/signal.hh>

namespace elle
{
  namespace reactor
  {
    namespace fsm
    {
      /// A State of the Machine.
      ///
      /// A State contains an Action to be executed when the Finite State
      /// Machine reaches it.
      ///
      /// \code{.cc}
      ///
      /// elle::reactor::Machine m;
      /// // Prefer m.state_make.
      /// auto state = std::make_unique<elle::reactor::State>(
      ///   []
      ///   {
      ///     std::cout << "Hello.";
      ///   });
      /// m.state_add(std::move(state));
      /// m.run();
      ///
      /// //Result: Hello.
      ///
      /// \endcode
      class State:
        public elle::Printable
      {
      /*------.
      | Types |
      `------*/
      public:
        using Action = std::function<void ()>;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        /// Create a State.
        ///
        /// \param name A descriptive name of the state.
        /// \param action The Action to be performed.
        explicit
        State(std::string name = "", Action action = [] () {});
        /// Create State.
        ///
        /// \param action The Action to be performed.
        explicit
        State(Action const& action);
        /// Create a State that does nothing.
        ///
        /// \param name A descriptive name of the state.
        explicit
        State(char const* name);
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
        using Transitions =
          boost::multi_index::multi_index_container<
            Transition*,
            boost::multi_index::indexed_by<
              boost::multi_index::hashed_unique<
                boost::multi_index::identity<Transition*>
              >,
              boost::multi_index::sequenced<>
            >
          >;
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
        void
        print(std::ostream& stream) const override;
      };
    }
  }
}
