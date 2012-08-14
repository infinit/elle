#include <ostream>

#include "elle/serialize/JSONArchive.hxx"

#include "String.hh"

using namespace elle::format::json;

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
        String::repr(std::ostream& out) const
        {
          out << '"';
          for (auto it = _value.cbegin(), end = _value.cend(); it != end; ++it)
            {
              switch (*it)
              {
              case '"':
                out << "\\\""; break;
              case '\\':
                out << "\\\\"; break;
              case '/':
                out << "\\/"; break;
              case '\b':
                out << "\\b"; break;
              case '\f':
                out << "\\f"; break;
              case '\n':
                out << "\\n"; break;
              case '\r':
                out << "\\r"; break;
              case '\t':
                out << "\\t"; break;
              default:
                out << *it; break;
              }
            }
          out << '"';
        }

      }
    }
  }
}
