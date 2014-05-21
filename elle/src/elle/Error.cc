#include <elle/Error.hh>

namespace elle
{
  Error::Error(std::string const& format)
    : Super(format)
  {}

  Error::Error(elle::Backtrace const& bt, std::string const& format)
    : Super(bt, format)
  {}
}
