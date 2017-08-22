#pragma once

#include <exception>
#include <string>

namespace elle
{
  class Backtrace;
  class Buffer;
  class ConstWeakBuffer;
  class Error;
  class IntRange;
  class IntRanges;
  class Version;
  class WeakBuffer;

  std::string
  exception_string(std::exception_ptr err);

  inline
  std::string
  exception_string()
  {
    return exception_string(std::current_exception());
  }
}

// Keep after the previous fwd decls.
#include <elle/compiler.hh>
#include <elle/format/fwd.hh>
#include <elle/network/fwd.hh>
#include <elle/serialization/fwd.hh>
