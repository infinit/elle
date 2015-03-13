#ifndef ELLE_SERIALIZATION_JSON_OVERFLOW_HH
# define ELLE_SERIALIZATION_JSON_OVERFLOW_HH

# include <elle/serialization/Error.hh>

namespace elle
{
  namespace serialization
  {
    namespace json
    {
      class Overflow
        : public serialization::Error
      {
      public:
        Overflow(std::string const& field, int size, bool over);
        ELLE_ATTRIBUTE_R(std::string, field);
      };
    }
  }
}

#endif
