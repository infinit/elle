#include <aws/Exceptions.hh>

namespace aws
{
  RequestError::RequestError(std::string const& error):
    AWSException(error)
  {}
}
