#include <openssl/crypto.h>
#include <openssl/dsa.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#include <elle/log.hh>

#include <elle/cryptography/Error.hh>
#include <elle/cryptography/context.hh>
#include <elle/cryptography/finally.hh>

namespace elle
{
  namespace cryptography
  {
    namespace context
    {
      /*----------.
      | Functions |
      `----------*/

      EVP_PKEY_CTX*
      create(::EVP_PKEY* key,
             int (*function)(EVP_PKEY_CTX*))
      {
        EVP_PKEY_CTX* context;

        ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY_CONTEXT(context);

        if ((context = ::EVP_PKEY_CTX_new(key, nullptr)) == nullptr)
          throw Error(
            elle::sprintf("unable to allocate a EVP_PKEY context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        if (function(context) <= 0)
          throw Error(
            elle::sprintf("unable to initialize the EVP_PKEY context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        ELLE_CRYPTOGRAPHY_FINALLY_ABORT(context);

        return (context);
      }
    }
  }
}
