#pragma once

#include <elle/Error.hh>

namespace elle
{
  /// Equivalent of std::bad_alloc.
  class BadAlloc
    : public Error
  {
  public:
    BadAlloc();
  };
}
