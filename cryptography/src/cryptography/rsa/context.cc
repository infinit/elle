#include <cryptography/rsa/context.hh>
#include <cryptography/finally.hh>
#include <cryptography/Exception.hh>

#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

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
          EVP_PKEY_CTX* context;

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY_CONTEXT(context);

          if ((context = ::EVP_PKEY_CTX_new(key, nullptr)) == nullptr)
            throw Exception(
              elle::sprintf("unable to allocate a EVP_PKEY context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          if (function(context) <= 0)
            throw Exception(
              elle::sprintf("unable to initialize the EVP_PKEY context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          int _padding = padding::resolve(padding);

          if (::EVP_PKEY_CTX_ctrl(context,
                                  EVP_PKEY_RSA,
                                  -1,
                                  EVP_PKEY_CTRL_RSA_PADDING,
                                  _padding,
                                  nullptr) <= 0)
            throw Exception(
              elle::sprintf("unable to set the EVP_PKEY context's padding: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(context);

          return (context);
        }
      }
    }
  }
}
