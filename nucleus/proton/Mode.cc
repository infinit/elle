#include <nucleus/proton/Mode.hh>

namespace nucleus
{
  namespace proton
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
        case Mode::encrypted:
          {
            stream << "encrypted";
            break;
          }
        case Mode::decrypted:
          {
            stream << "decrypted";
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
