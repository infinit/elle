#ifndef  ELLE_EXCEPTION_HH
# define ELLE_EXCEPTION_HH

# include <reactor/exception.hh>

# include <elle/types.hh>
# include <elle/printf.hh>
# include <elle/concurrency/Scheduler.hh>

namespace elle
{

  class Exception:
    public reactor::Exception
  {
  public:
    template<typename... Args>
    Exception(elle::String const& fmt, Args&&... args):
      reactor::Exception(elle::concurrency::scheduler(),
                         elle::sprintf(fmt.c_str(),
                                       std::forward<Args>(args)...) + _add_report())
    {}

    static
    elle::String
    _add_report();

  };

}

#endif

