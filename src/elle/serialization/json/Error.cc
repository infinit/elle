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

    TypeError::TypeError(std::string field,
                         std::string expected,
                         std::string effective)
      : Error(
        elle::sprintf(
          "invalid type for key \"%s\", expected a %s got a %s",
          field, expected, effective))
      , _field(std::move(field))
      , _expected(std::move(expected))
      , _effective(std::move(effective))
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
