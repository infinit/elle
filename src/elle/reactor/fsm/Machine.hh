#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include <boost/signals2.hpp>

#include <elle/Printable.hh>

#include <elle/reactor/fsm/State.hh>
#include <elle/reactor/fsm/Transition.hh>

namespace elle
{
  namespace reactor
  {
    namespace fsm
    {
      /// Reactorified implementation of finite state machines.
      ///
      /// The Machine stores a collection of States and a collection of
      /// Transitions. Machine takes care of checking conditions, switching
      /// State, etc.
      ///
      /// Machine::run is the entry point of the FSM, that starts and keep it
      /// alive until it reaches State a final State (one that has no
      /// transitions)
      ///
      /// \code{.cc}
      ///
      /// elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope &s)
      /// {
      ///   elle::reactor::Signal a2b;
      ///   elle::reactor::Signal b2c;
      ///
      ///   s.run_background("fsm",
      ///     [&]
      ///     {
      ///       elle::reactor::fsm::Machine m("ABC");
      ///       auto& a = m.state_make("A", [&] {
      ///          std::cout << "A";
      ///         });
      ///       State& b = m.state_make("b", [&] {
      ///          std::cout << "B";
      ///         });
      ///       State& C = m.state_make("c", [&] {
      ///          std::cout << "C";
      ///         });
      ///       m.transition_add(a, b, elle::reactor::Waitables{&a2b});
      ///       m.transition_add(b, c, elle::reactor::Waitables{&b2c});
      ///       m.run(a);
      ///    });
      ///
      ///   s.run_background("trigger",
      ///     []
      ///     {
      ///       std::cout << "1";
      ///       reactor::sleep(100_ms);
      ///       std::cout << "2";
      ///       a2b.signal();
      ///       reactor::sleep(100_ms);
      ///       std::cout << "3";
      ///       b2c.signal();
      ///     });
      ///   s.wait();
      /// };
      /// // Result: 1A2B3C
      ///
      /// \encode
      class Machine
        : public elle::Printable
      {
      /*------.
      | Types |
      `------*/
      public:
        using PreTrigger = std::function<bool ()>;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        /// Construct a Machine.
        ///
        /// \param name Descriptive name.
        Machine(std::string name = "");

      /*-------.
      | States |
      `-------*/
      public:
        /// Make a State.
        ///
        /// State is owned by the current Machine.
        ///
        /// \tparams Args Types of arguments to construct the State.
        /// \params ... Arguments used to construct the State.
        template <typename ... Args>
        State&
        state_make(Args&& ...);
        /// Add an existing state to the Machine. It takes the ownership.
        ///
        /// \param state Pointer to a State.
        /// \returns Reference to the state, now owned by the Machine.
        State&
        state_add(std::unique_ptr<State> state);
      private:
        ELLE_ATTRIBUTE(std::unordered_set<std::unique_ptr<State>>, states);
        ELLE_ATTRIBUTE_R(State*, start);

      /*------------.
      | Transitions |
      `------------*/
      public:
        /// Add a transition between two States, triggered by Waitables.
        ///
        /// \pre States must be owned by the Machine.
        ///
        /// @see WaitableTransition::WaitableTransition.
        Transition&
        transition_add(
          State& start,
          State& end,
          Waitables const& trigger,
          bool preemptive = false,
          PreTrigger const& pre_trigger = PreTrigger());
        /// Add a normal Transition between two States. The condition for
        /// changing from State start to State end is the end of State start.
        ///
        /// \pre States must be owned by the Machine.
        ///
        /// \param start The source State of the Transition.
        /// \param end The destination State of the Transition.
        /// \returns A reference to the Transition added.
        Transition&
        transition_add(State& start, State& end);
        /// Add a normal Transition between two States. Like
        /// transition_add(start, end) but with an extra condition.
        ///
        /// \pre States must be owned by the Machine.
        ///
        /// \param start The source State of the Transition.
        /// \param end The destination State of the Transition.
        /// \param condition The condition to fulfilled.
        /// \returns A reference to the Transition added.
        Transition&
        transition_add(State& start, State& end,
                       std::function<bool ()> const& condition);
        /// Add a transition upon exception.
        ///
        /// Add a transition that will switch execution to state \a end if an
        /// elle::Exception is thrown during the execution of state \a start.
        ///
        /// \param start  The state where to catch transition.
        /// \param end The state to switch to.
        /// \returns A reference to transition added.
        Transition&
        transition_add_catch(State& start, State& end);
        /// Add a transition upon exception.
        ///
        /// Add a transition that will switch execution to state \a end if an
        /// exception of type\a T is thrown during the execution of state \a
        /// start.
        ///
        /// \param start The state where to catch transition.
        /// \param end The state to switch to.
        /// \param front (optional) If the transition should be push front or
        ///               normally inserted at the end of the list.
        /// \returns A reference to the transition added.
        template <typename T>
        Transition&
        transition_add_catch_specific(State& start, State& end,
                                      bool front = false);
      private:
        ELLE_ATTRIBUTE(
          std::unordered_set<std::unique_ptr<Transition>>, transitions);

      /*----------.
      | Execution |
      `----------*/
      public:
        /// Start the Machine, wait until a final State is over.
        void
        run();
        /// Start the Machine at a specific State, wait until a final State is
        /// over.
        void
        run(State& start);
      private:
        State*
        _run_state(State* state);
        ELLE_ATTRIBUTE_R(bool, running);
        ELLE_ATTRIBUTE_R(std::exception_ptr, exception);
        ELLE_ATTRIBUTE_RX(boost::signals2::signal<void (State&)>,
                          state_changed);
        ELLE_ATTRIBUTE_RX(boost::signals2::signal<void (Transition&)>,
                          transition_triggered);
        ELLE_ATTRIBUTE_R(State*, current_state);

      /*----------.
      | Printable |
      `----------*/
      public:
        virtual
        void
        print(std::ostream& stream) const;
        ELLE_ATTRIBUTE_R(std::string, name);
      };
    }
  }
}

#include <elle/reactor/fsm/Machine.hxx>
