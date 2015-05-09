#ifndef INFINIT_CRYPTOGRAPHY_ONEWAY_HH
# define INFINIT_CRYPTOGRAPHY_ONEWAY_HH

# include <cryptography/fwd.hh>

# include <elle/types.hh>

# include <openssl/evp.h>

# include <iosfwd>

namespace infinit
{
  namespace cryptography
  {
    /*-------------.
    | Enumerations |
    `-------------*/

    /// Define the oneway algorithm.
    enum class Oneway
    {
      md5,
      sha,
      sha1,
      sha224,
      sha256,
      sha384,
      sha512
    };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Oneway const oneway);

    namespace oneway
    {
      /*----------.
      | Functions |
      `----------*/

      /// Resolve a algorithm name into an EVP function pointer.
      ::EVP_MD const*
      resolve(Oneway const name);
      /// Return the name of the oneway function.
      Oneway
      resolve(::EVP_MD const* function);
    }
  }
}

#endif
