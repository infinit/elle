#ifndef ELLE_BACKTRACE_HXX
# define ELLE_BACKTRACE_HXX

# if not defined(INFINIT_WINDOWS) and not defined(INFINIT_ANDROID)
#   ifdef __has_include
#     if __has_include(<execinfo.h>)
#       include <execinfo.h>
#     else
#       define NO_EXECINFO
#     endif
#   else
#     include<execinfo.h>
#   endif
# endif

namespace elle
{
  inline
  Backtrace
  Backtrace::current(unsigned skip)
  {
# if defined(INFINIT_WINDOWS)
    return Backtrace::_current();
# elif defined(INFINIT_ANDROID) || defined(NO_EXECINFO)
    // FIXME: implement with https://android.googlesource.com/platform/frameworks/native/+/jb-dev/include/utils/CallStack.h
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
