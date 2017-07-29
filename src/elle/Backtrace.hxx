#include <elle/log.hh>

namespace elle
{
  inline
  Backtrace::Backtrace(now_t, unsigned skip)
  {
    ELLE_LOG_COMPONENT("elle.Backtrace");
#if ELLE_HAVE_BACKTRACE
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
