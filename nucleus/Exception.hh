#ifndef NUCLEUS_EXCEPTION_HH
# define NUCLEUS_EXCEPTION_HH

# include <elle/types.hh>

# include <elle/idiom/Close.hh>
#  include <reactor/exception.hh>
# include <elle/idiom/Open.hh>

namespace nucleus
{

  class Exception:
    public reactor::Exception
  {
    //
    // construction
    //
  public:
    Exception(elle::String const& message);
    ~Exception() throw();
  };

}

#endif
