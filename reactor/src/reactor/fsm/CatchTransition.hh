#pragma once

#include <reactor/fsm/Transition.hh>
#include <reactor/fsm/fwd.hh>

namespace reactor
{
  namespace fsm
  {
    class CatchTransition:
      public Transition
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef CatchTransition Self;
      typedef Transition Super;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      virtual
      void
      done(Transition*& trigger, std::exception_ptr& exn) override;
      ELLE_ATTRIBUTE_RW(std::function<bool (std::exception_ptr const&)>,
                        condition);
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
