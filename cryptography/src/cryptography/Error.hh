#ifndef INFINIT_CRYPTOGRAPHY_ERROR_HH
# define INFINIT_CRYPTOGRAPHY_ERROR_HH

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
      Error(std::string const& message);
      Error(elle::serialization::SerializerIn& input);
    };
  }
}

#endif
