#pragma once

#include <elle/serialization/Error.hh>

namespace elle
{
  namespace serialization
  {
    namespace json
    {
      /// Serialization error related to a specific JSON field.
      class FieldError
        : public Error
      {
      public:
        using Self = FieldError;
        using Super = Error;
        FieldError(std::string const& field, std::string const& message);
        ELLE_ATTRIBUTE_R(std::string, field);
      };
    }
  }
}
