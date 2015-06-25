#ifndef INFINIT_CRYPTOGRAPHY_RSA_SERIALIZATION_HH
# define INFINIT_CRYPTOGRAPHY_RSA_SERIALIZATION_HH

# include <elle/serialization/Serializer.hh>

# include <openssl/rsa.h>

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /*----------.
      | Functions |
      `----------*/

      /// Serialize the public components of an RSA key.
      void
      serialize_public(elle::serialization::Serializer& serializer,
                       ::RSA*& rsa);
      /// Serialize the private components of an RSA key.
      void
      serialize_private(elle::serialization::Serializer& serializer,
                        ::RSA*& rsa);
    }
  }
}

#endif
