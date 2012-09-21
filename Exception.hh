#ifndef  ELLE_EXCEPTION_HH
# define ELLE_EXCEPTION_HH

# include <reactor/exception.hh>

# include <elle/concurrency/Scheduler.hh>
# include <elle/printf.hh>

namespace elle
{

  class Exception:
    public reactor::Exception
  {
  public:
    template<typename... Args>
    Exception(char const* fmt, Args&... args):
      reactor::Exception(elle::concurrency::scheduler(),
                         elle::sprintf(fmt, args...) + _add_report())
    {}

    static
    elle::String
    _add_report();

  };

}

#endif

