#include <nucleus/proton/State.hh>
#include <nucleus/Exception.hh>

#include <iostream>
#include <stdexcept>

namespace nucleus
{
  namespace proton
  {
    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                State const state)
    {
      switch (state)
        {
        case State::clean:
          {
            stream << "clean";
            break;
          }
        case State::dirty:
          {
            stream << "dirty";
            break;
          }
        case State::consistent:
          {
            stream << "consistent";
            break;
          }
        default:
          {
            throw Exception("unknown state: '%s'", static_cast<int>(state));
          }
        }

      return (stream);
    }
  }
}
