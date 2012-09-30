#include <nucleus/neutron/Permissions.hh>
#include <nucleus/Exception.hh>

#include <iostream>
#include <stdexcept>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- operators -------------------------------------------------------
//

    std::ostream&
    operator <<(std::ostream& stream,
                Permissions const permissions)
    {
      switch (permissions)
        {
        case permissions::none:
          {
            stream << "none";
            break;
          }
        case permissions::read:
          {
            stream << "read";
            break;
          }
        case permissions::write:
          {
            stream << "write";
            break;
          }
        default:
          {
            throw Exception("unknown permissions '%s'", permissions);
          }
        }

      return (stream);
    }

  }
}
