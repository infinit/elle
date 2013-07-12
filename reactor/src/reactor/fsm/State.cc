#include <reactor/fsm/State.hh>

namespace reactor
{
  namespace fsm
  {
    /*-------------.
    | Construction |
    `-------------*/

    State::State(Action const& action):
      State("", action)
    {}

    State::State(char const* name):
      State(std::string(name))
    {}

    State::State(std::string const& name, Action const& action):
      _action(action),
      _name(name)
    {}

    /*----------.
    | Printable |
    `----------*/

    void
    State::print(std::ostream& stream) const
    {
      stream << "State " << this->name();
    }
  }
}
