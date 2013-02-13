#include <ostream>

#include "elle/serialize/JSONArchive.hxx"

#include "Integer.hh"

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
        Integer::repr(std::ostream& out) const
        {
          out << _value;
        }

      }
    }
  }
}

