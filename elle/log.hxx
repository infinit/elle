#ifndef  ELLE_LOG_HXX
# define ELLE_LOG_HXX

# include <elle/printf.hh>

namespace elle
{
  namespace log
  {
    namespace detail
    {
      inline
      TraceContext::operator bool() const
      {
        return false;
      }
    }

  }
}

#endif
