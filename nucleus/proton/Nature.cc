#include <nucleus/proton/Nature.hh>

namespace nucleus
{
  namespace neutron
  {
    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Nature const nature)
    {
      switch (nature)
        {
        case Nature::empty:
          {
            stream << "empty";
            break;
          }
        case Nature::value:
          {
            stream << "value";
            break;
          }
        case Nature::block:
          {
            stream << "block";
            break;
          }
        case Nature::tree:
          {
            stream << "tree";
            break;
          }
        default:
          {
            throw Exception("unknown nature: '%s'", static_cast<int>(nature));
          }
        }

      return (stream);
    }
  }
}
