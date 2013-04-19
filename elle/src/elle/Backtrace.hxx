#ifndef ELLE_BACKTRACE_HXX
# define ELLE_BACKTRACE_HXX

# include <execinfo.h>

namespace elle
{
  inline
  Backtrace
  Backtrace::current(unsigned skip)
  {
    static const size_t size = 128;
    void* callstack[size];
    size_t frames = ::backtrace(callstack, size);
    return Backtrace::_current(callstack, frames, skip);
  }
}

#endif
