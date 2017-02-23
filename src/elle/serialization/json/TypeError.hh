#ifndef ELLE_SERIALIZATION_JSON_TYPEERROR_HH
# define ELLE_SERIALIZATION_JSON_TYPEERROR_HH

# include <typeinfo>

# include <elle/serialization/Error.hh>

namespace elle
{
  namespace serialization
  {
    class TypeError:
      public Error
    {
    public:
      TypeError(std::string const& field,
                std::type_info const& expected,
                std::type_info const& effective);
      ELLE_ATTRIBUTE_R(std::string, field);
      ELLE_ATTRIBUTE_R(std::type_info const*, expected);
      ELLE_ATTRIBUTE_R(std::type_info const*, effective);
    };
  }
}

#endif
