#include <ostream>

#include "Float.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {
      namespace detail
      {

        template<>
        void
        Float::repr(std::ostream& out) const
        {
          out << _value;
        }

      }
    }
  }
}
