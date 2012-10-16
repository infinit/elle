#ifndef ELLE_EXCEPTION_HXX
# define ELLE_EXCEPTION_HXX

# include <elle/printf.hh>
# include <elle/concurrency/Scheduler.hh>

namespace elle
{

  /*-------------.
  | Construction |
  `-------------*/

  template <typename... Args>
  Exception::Exception(elle::String const& format,
                       Args&&... args):
    reactor::Exception(elle::concurrency::scheduler(),
                       elle::sprintf(format.c_str(),
                                     std::forward<Args>(args)...))
  {
  }

}

#endif
