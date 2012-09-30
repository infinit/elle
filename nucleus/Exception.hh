#ifndef NUCLEUS_EXCEPTION_HH
# define NUCLEUS_EXCEPTION_HH

# include <elle/types.hh>
# include <elle/Exception.hh>

namespace nucleus
{

  class Exception:
    public elle::Exception
  {
    //
    // construction
    //
  public:
    template <typename... A>
    Exception(elle::String const& format,
              A&&... arguments);
  };

}

# include <nucleus/Exception.hxx>

#endif
