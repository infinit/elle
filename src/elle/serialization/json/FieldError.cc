#include <elle/printf.hh>
#include <elle/serialization/json/FieldError.hh>

namespace elle
{
  namespace serialization
  {
    namespace json
    {
      FieldError::FieldError(std::string const& field,
                             std::string const& message)
        : Super(elle::sprintf("field \"%s\": %s", field, message))
        , _field(field)
      {}
    }
  }
}
