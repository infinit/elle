#include <aws/Exceptions.hh>

#include <elle/printf.hh>

namespace aws
{
  AWSException::AWSException(std::string const& error):
    Super(elle::sprintf("AWS error: %s", error)),
    _error(error)
  {}

  RequestError::RequestError(std::string const& error):
    AWSException(error)
  {}
}
