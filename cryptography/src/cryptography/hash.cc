#include <cryptography/hash.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/evp.hh>
#include <cryptography/Exception.hh>
#include <cryptography/Oneway.hh>

#include <elle/log.hh>

#include <openssl/err.h>

ELLE_LOG_COMPONENT("infinit.cryptography.hash");

namespace infinit
{
  namespace cryptography
  {
    /*----------.
    | Functions |
    `----------*/

    Digest
    hash(Plain const& plain,
         Oneway oneway)
    {
      ELLE_TRACE_FUNCTION(oneway);
      ELLE_DUMP("plain: %x", plain);

      ::EVP_MD const* function = oneway::resolve(oneway);

      return (Digest(evp::digest::hash(plain.buffer(), function)));
    }
  }
}
