#pragma once

#include <elle/reactor/fsm/Transition.hh>
#include <elle/reactor/fsm/fwd.hh>

namespace elle
{
  namespace reactor
  {
    namespace fsm
    {
      /// A specialized Transition that occurs when an exception is thrown from
      /// the origin State.
      class CatchTransition
        : public Transition
      {
      /*------.
      | Types |
      `------*/
      public:
        using Self = CatchTransition;
        using Super = Transition;
        using Condition = std::function<bool (std::exception_ptr const&)>;
      /*-------------.
      | Construction |
      `-------------*/
      public:
        /// If the State is over because of an exception and this exception is
        /// relevant for our condition, set the trigger to this.
        ///
        /// \param trigger Who trigger the Transition. If condition(exn) is
        ///                true, set trigger = this and reset exn.
        /// \param exn An exception pointer.
        ///
        /// \post if exn is not null and condition(exn) is true,
        ///       trigger == this.
        void
        done(Transition*& trigger, std::exception_ptr& exn) override;
        ELLE_ATTRIBUTE_RW(Condition, condition);
      protected:
        CatchTransition(State& start,
                        State& end,
                        bool make_first = false);
        friend class Machine;

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
}
