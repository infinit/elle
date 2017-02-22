#include <elle/printf.hh>
#include <elle/serialization/json/TypeError.hh>

namespace elle
{
  namespace serialization
  {
    TypeError::TypeError(std::string const& field,
                         std::type_info const& expected,
                         std::type_info const& effective)
      : Error(
        elle::sprintf(
          "invalid type for key \"%s\", expected a %s got a %s",
          field,
          elle::demangle(expected.name()),
          elle::demangle(effective.name())))
      , _field(field)
      , _expected(&expected)
      , _effective(&effective)
    {}
  }
}
