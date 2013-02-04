#ifndef CRYPTOGRAPHY_EXCEPTION_HH
# define CRYPTOGRAPHY_EXCEPTION_HH

# include <elle/types.hh>
# include <elle/Exception.hh>

namespace cryptography
{
  /// Represent an exception triggered following a cryptographic operation.
  class Exception:
    public elle::Exception
  {
    /*-------------.
    | Construction |
    `-------------*/
  public:
    template <typename... A>
    Exception(elle::String const& format,
              A&&... arguments);
  };
}

# include <cryptography/Exception.hxx>

#endif
