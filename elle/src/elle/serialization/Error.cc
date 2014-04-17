#include <elle/serialization/Error.hh>


namespace elle
{
  namespace serialization
  {
    Error::Error(std::string const& message)
      : Exception(message)
    {}
  }
}
