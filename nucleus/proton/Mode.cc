#include <nucleus/proton/Mode.hh>

namespace nucleus
{
  namespace neutron
  {
    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Mode const mode)
    {
      switch (mode)
        {
        case Mode::empty:
          {
            stream << "empty";
            break;
          }
        case Mode::value:
          {
            stream << "value";
            break;
          }
        case Mode::block:
          {
            stream << "block";
            break;
          }
        case Mode::tree:
          {
            stream << "tree";
            break;
          }
        default:
          {
            throw Exception("unknown mode: '%s'", static_cast<int>(mode));
          }
        }

      return (stream);
    }
  }
}
