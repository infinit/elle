#include <reactor/exception.hh>

namespace reactor
{
  Exception::Exception(const std::string& message)
    : std::runtime_error(message)
    , _backtrace()
  {}

  Exception::~Exception() throw ()
  {}

  Backtrace const&
  Exception::backtrace() const
  {
    return _backtrace;
  }

  Terminate::Terminate()
    : Super("thread termination")
  {}
}
