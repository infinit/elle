#include <elle/json/exceptions.hh>

namespace elle
{
  namespace json
  {
    ParserError::ParserError(std::string const& message):
      elle::Exception(message)
    {}
  }
}