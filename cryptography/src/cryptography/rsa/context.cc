#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

#include <cryptography/Error.hh>
#include <cryptography/context.hh>
#include <cryptography/finally.hh>
#include <cryptography/rsa/context.hh>

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace context
      {
        /*----------.
        | Functions |
        `----------*/

        EVP_PKEY_CTX*
        create(::EVP_PKEY* key,
               int (*function)(EVP_PKEY_CTX*),
               Padding const padding)
        {
          EVP_PKEY_CTX* context = cryptography::context::create(key, function);

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY_CONTEXT(context);

          int _padding = padding::resolve(padding);

          if (EVP_PKEY_CTX_set_rsa_padding(context, _padding) <= 0)
            throw Error(
              elle::sprintf("unable to set the EVP_PKEY context's "
                            "padding '%s': %s",
                            _padding,
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(context);

          return (context);
        }
      }
    }
  }
}
