#ifndef ELLE_BACKTRACE_HXX
# define ELLE_BACKTRACE_HXX

# ifndef INFINIT_WINDOWS
#  include <execinfo.h>
# endif

namespace elle
{
  inline
  Backtrace
  Backtrace::current(unsigned skip)
  {
# ifdef INFINIT_WINDOWS
    return Backtrace();
# else
    static const size_t size = 128;
    void* callstack[size];
    size_t frames = ::backtrace(callstack, size);
    return Backtrace::_current(callstack, frames, skip);
# endif
  }
}

#endif
