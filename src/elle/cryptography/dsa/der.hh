#ifndef ELLE_CRYPTOGRAPHY_DSA_DER_HH
# define ELLE_CRYPTOGRAPHY_DSA_DER_HH

# include <openssl/dsa.h>

# include <elle/Buffer.hh>

namespace elle
{
  namespace cryptography
  {
    namespace dsa
    {
      /// Distinguished Encoding Rules is a restricted variant of BER for
      /// producing unequivocal transfer syntax for data structures described
      /// by ASN.1.
      namespace der
      {
        /*----------.
        | Functions |
        `----------*/

        /// Encode the given DSA key's public part into a binary-based format.
        elle::Buffer
        encode_public(::DSA* dsa);
        /// Decode the DSA public key from a DER-based buffer.
        ::DSA*
        decode_public(elle::ConstWeakBuffer const& buffer);
        /// Encode the given DSA key's private part into a binary-based format.
        elle::Buffer
        encode_private(::DSA* dsa);
        /// Decode the DSA private key from a DER-based buffer.
        ::DSA*
        decode_private(elle::ConstWeakBuffer const& buffer);
      }
    }
  }
}

#endif
