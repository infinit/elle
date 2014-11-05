#ifndef ELLE_SERIALIZATION_JSON_FIELD_ERROR_HH
# define ELLE_SERIALIZATION_JSON_FIELD_ERROR_HH

# include <elle/serialization/Error.hh>

namespace elle
{
  namespace serialization
  {
    namespace json
    {
      /// Serialization error related to a specific JSON field.
      class FieldError:
        public Error
      {
      public:
        typedef FieldError Self;
        typedef Error Super;
        FieldError(std::string const& field, std::string const& message);
        ELLE_ATTRIBUTE_R(std::string, field);
      };
    }
  }
}

#endif
