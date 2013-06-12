#include <ostream>
#include <iomanip>

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
          out << std::setprecision(16) << this->_value;
        }
      }
    }
  }
}
