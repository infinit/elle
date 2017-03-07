#include <elle/reactor/fsm/State.hh>
#include <utility>

namespace elle
{
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

      State::State(std::string  name, Action  action):
        _action(std::move(action)),
        _name(std::move(name))
      {}

      /*----------.
      | Printable |
      `----------*/

      void
      State::print(std::ostream& stream) const
      {
        stream << "State(";
        if (!this->name().empty())
          stream << this->name();
        else
          stream << this;
        stream << ')';
      }
    }
  }
}
