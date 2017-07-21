#if !defined ELLE_WINDOWS and !defined ELLE_ANDROID
#  ifdef __has_include
#    if __has_include(<execinfo.h>)
#      include <execinfo.h>
#    else
#      define NO_EXECINFO
#    endif
#  else
#    include <execinfo.h>
#  endif
#endif

#include <elle/log.hh>

namespace elle
{
  inline
  Backtrace::Backtrace(now_t, unsigned skip)
  {
    ELLE_LOG_COMPONENT("elle.Backtrace");
#if defined ELLE_WINDOWS
    {};
#elif defined ELLE_ANDROID || defined NO_EXECINFO
    // FIXME: implement with
    // https://android.googlesource.com/platform/frameworks/native/+/jb-dev/include/utils/CallStack.h
    {};
#else
    this->_frame_count = ::backtrace(this->_callstack.data(),
                                     this->_callstack.size());
    ELLE_DEBUG("backtrace returned %s frames",
               this->_frame_count);
    this->_skip = skip;
#endif
  }

  inline
  Backtrace
  Backtrace::current(unsigned skip)
  {
    return {now, skip};
  }
}
