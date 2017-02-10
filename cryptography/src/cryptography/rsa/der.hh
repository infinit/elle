#ifndef ELLE_CRYPTOGRAPHY_RSA_DER_HH
# define ELLE_CRYPTOGRAPHY_RSA_DER_HH

# include <openssl/rsa.h>

# include <elle/types.hh>
# include <elle/Buffer.hh>

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      /// Distinguished Encoding Rules is a restricted variant of BER for
      /// producing unequivocal transfer syntax for data structures described
      /// by ASN.1.
      namespace der
      {
        /*----------.
        | Functions |
        `----------*/

        /// Encode the given RSA key's public part into a binary-based format.
        elle::Buffer
        encode_public(::RSA* rsa);
        /// Decode the RSA public key from a DER-based buffer.
        ::RSA*
        decode_public(elle::ConstWeakBuffer const& buffer);
        /// Encode the given RSA key's private part into a binary-based format.
        elle::Buffer
        encode_private(::RSA* rsa);
        /// Decode the RSA private key from a DER-based buffer.
        ::RSA*
        decode_private(elle::ConstWeakBuffer const& buffer);
      }
    }
  }
}

#endif
