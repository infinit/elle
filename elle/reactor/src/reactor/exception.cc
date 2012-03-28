#include <reactor/exception.hh>

namespace reactor
{
  Exception::Exception(const std::string& message)
    : std::runtime_error(message)
  {}
}
