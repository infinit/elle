#include <elle/printf.hh>
#include <elle/serialization/json/Error.hh>

namespace elle
{
  namespace serialization
  {
    MissingKey::MissingKey(std::string const& field)
      : Error(elle::sprintf("missing mandatory key: \"%s\"", field))
      , _field(field)
    {}

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

    namespace json
    {
      FieldError::FieldError(std::string const& field,
                             std::string const& message)
        : Super(elle::sprintf("field \"%s\": %s", field, message))
        , _field(field)
      {}

      Overflow::Overflow(std::string const& field, int size, bool over)
        : Error(elle::sprintf("%s-bit %s on field \"%s\"",
                              size, over ? "overflow" : "underflow", field))
        , _field(field)
      {}
    }
  }
}
