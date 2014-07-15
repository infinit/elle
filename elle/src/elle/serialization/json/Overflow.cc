#include <elle/printf.hh>
#include <elle/serialization/json/Overflow.hh>

namespace elle
{
  namespace serialization
  {
    namespace json
    {
      Overflow::Overflow(std::string const& field, int size, bool over)
        : Error(elle::sprintf("%s-bits %sflow on field: \"%s\"",
                              size, over ? "over" : "under", field))
        , _field(field)
      {}
    }
  }
}
