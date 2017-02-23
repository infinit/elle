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
      class Machine:
        public elle::Printable
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
        Machine(std::string const& name = "");

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
        transition_add(State& start, State& end);
        Transition&
        transition_add(State& start, State& end,
                       std::function<bool ()> const& condition);
        /// Add a transition upon exception.
        ///
        /// Add a transition that will switch execution to state \a end if an
        /// elle::Exception is thrown during the execution of state \a start.
        ///
        /// \param  start The state where to catch transition.
        /// \param  end   The state to switch to.
        /// \return       The added transition.
        Transition&
        transition_add_catch(State& start, State& end);
        /// Add a transition upon exception.
        ///
        /// Add a transition that will switch execution to state \a end if an
        /// exception of type\a T is thrown during the execution of state \a
        /// start.
        ///
        /// \param  start The state where to catch transition.
        /// \param  end   The state to switch to.
        /// \param  front (optional) If the transition should be push front or
        ///               normally inserted at the end of the list.
        /// \return       The added transition.
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
        void
        run();
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
