#include <elle/err.hh>

#include <elle/Error.hh>

namespace elle
{
  void
  err(std::string const& msg)
  {
    throw elle::Error(msg);
  }
}
