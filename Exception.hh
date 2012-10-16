#ifndef ELLE_EXCEPTION_HH
# define ELLE_EXCEPTION_HH

# include <reactor/exception.hh>

# include <elle/types.hh>

namespace elle
{

  /// This class abstracts a reactor exception for the current scheduler.
  class Exception:
    public reactor::Exception
  {
    /*-------------.
    | Construction |
    `-------------*/
  public:
    template <typename... Args>
    Exception(elle::String const& format,
              Args&&... args);
  };

}

# include <elle/Exception.hxx>

#endif
