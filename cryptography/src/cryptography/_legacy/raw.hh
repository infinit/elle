#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
# ifndef INFINIT_CRYPTOGRAPHY_LEGACY_RAW_HH
#  define INFINIT_CRYPTOGRAPHY_LEGACY_RAW_HH

#  include <cryptography/fwd.hh>
#  include <cryptography/Cipher.hh>
#  include <cryptography/Oneway.hh>

#  include <elle/types.hh>
#  include <elle/fwd.hh>

#  include <openssl/evp.h>

#  include <memory>

//
// ---------- Asymmetric ------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace raw
    {
      namespace asymmetric
      {
        /*----------.
        | Functions |
        `----------*/

        /// Sign the given digest.
        elle::Buffer
        sign(elle::ConstWeakBuffer const& digest,
             ::EVP_PKEY_CTX* context,
             int (*function)(EVP_PKEY_CTX*,
                             unsigned char*,
                             size_t*,
                             const unsigned char*,
                             size_t));
        /// Return true if the signature is valid according to the given digest.
        elle::Boolean
        verify(elle::ConstWeakBuffer const& signature,
               elle::ConstWeakBuffer const& digest,
               ::EVP_PKEY_CTX* context,
               int (*function)(EVP_PKEY_CTX*,
                               const unsigned char*,
                               size_t,
                               const unsigned char*,
                               size_t));
      }
    }
  }
}

# endif
#endif
