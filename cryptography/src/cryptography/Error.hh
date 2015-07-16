#ifndef INFINIT_CRYPTOGRAPHY_EXCEPTION_HH
# define INFINIT_CRYPTOGRAPHY_EXCEPTION_HH

# include <elle/types.hh>
# include <elle/Error.hh>

namespace infinit
{
  namespace cryptography
  {
    /// An error triggered by a cryptographic operation.
    class Error:
      public elle::Error
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      Error(elle::String const& message);
    };
  }
}

#endif
