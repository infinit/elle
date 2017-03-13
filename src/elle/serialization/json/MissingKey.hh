#ifndef ELLE_SERIALIZATION_JSON_MISSINGKEY_HH
# define ELLE_SERIALIZATION_JSON_MISSINGKEY_HH

# include <typeinfo>

# include <elle/serialization/Error.hh>

namespace elle
{
  namespace serialization
  {
    /// Error related to a missing key.
    class MissingKey
      : public Error
    {
    public:
      MissingKey(std::string const& field);
      ELLE_ATTRIBUTE_R(std::string, field);
    };
  }
}

#endif
