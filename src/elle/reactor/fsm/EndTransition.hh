#pragma once

#include <elle/reactor/fsm/Transition.hh>
#include <elle/reactor/fsm/fwd.hh>

namespace elle
{
  namespace reactor
  {
    namespace fsm
    {
      /// A specialized Transition that occurs at the end of the source State.
      class EndTransition
        : public Transition
      {
      public:
        using Condition = std::function<bool ()>;
      public:
        /// Called when the origin State is over.
        ///
        /// \param trigger Who trigger the transition. If null, exn is null
        ///                and our condition is true or null, set
        ///                trigger = this.
        /// \param exn An exception pointer.
        virtual
        void
        done(Transition*& trigger, std::exception_ptr& exn) override;
      protected:
        /// Create a EndTransition.
        ///
        /// \param start The source State of the Transition.
        /// \param end The destination State of the Transition.
        EndTransition(State& start,
                      State& end);
        /// Create a EndTransition.
        ///
        /// \param start The source State of the Transition.
        /// \param end The destination State of the Transition.
        /// \param condition The Condition to trigger the Transition.
        EndTransition(State& start,
                      State& end,
                      Condition const& condition);
        friend class Machine;
        ELLE_ATTRIBUTE(boost::optional<Condition>, condition);

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
