#include <ostream>

#include "Bool.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      Bool const true_{true};
      Bool const false_{false};

      namespace detail
      {

        template<>
        void
        Bool::repr(std::ostream& out) const
        {
          out << (_value ? "true" : "false");
        }

      }
    }
  }
}

