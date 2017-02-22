#include <elle/json/exceptions.hh>

namespace elle
{
  namespace json
  {
    ParseError::ParseError(std::string const& message):
      Super(message)
    {}
  }
}
