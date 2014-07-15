#include <elle/serialization/Error.hh>


namespace elle
{
  namespace serialization
  {
    Error::Error(std::string const& message)
      : elle::Error(message)
    {}
  }
}
