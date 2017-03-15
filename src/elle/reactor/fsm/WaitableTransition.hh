#pragma once

#include <elle/reactor/fsm/fwd.hh>
#include <elle/reactor/fsm/Transition.hh>

namespace elle
{
  namespace reactor
  {
    namespace fsm
    {
      /// Specialization of Transition that waits on a Trigger.
      ///
      /// XXX[doc]: Add an example.
      class WaitableTransition
        : public Transition
      {
      public:
        using PreTrigger = std::function<bool ()>;
        /// Specialization of run.
        ///
        /// If pre_trigger is set, trigger (if not already done) and go directly
        /// to the next State (return boost::none).
        /// Otherwise, return a function that will wait for the trigger.
        ///
        /// In both cases, if preemptive is set, terminate origin State.
        boost::optional<Action>
        run(reactor::Signal& triggered,
            Transition*& trigger,
            Thread& action_thread) override;
      protected:
        /// Construct a WaitableTransition.
        ///
        /// \param start The source State of the Transition.
        /// \param end the destination State of the Transition.
        /// \param trigger The Waitables to trigger the condition.
        /// \param preemptive Whether if the trigger should interrupt the
        ///                   current flow of the State. Otherwise, the
        ///                   transition occurs at the end of the State.
        /// \param pre_trigger The Condition for pre triggering a Transition.
        WaitableTransition(State& start,
                           State& end,
                           Waitables  trigger,
                           bool preemptive = false,
                           PreTrigger  pre_trigger = PreTrigger());
        friend class Machine;
        ELLE_ATTRIBUTE_R(Waitables, trigger);
        ELLE_ATTRIBUTE_R(bool, preemptive);
        ELLE_ATTRIBUTE_R(PreTrigger, pre_trigger);

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
