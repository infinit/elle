#include <cryptography/low.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/Exception.hh>

#include <elle/log.hh>

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

ELLE_LOG_COMPONENT("infinit.cryptography.low");

namespace infinit
{
  namespace cryptography
  {
    namespace low
    {
      /*----------.
      | Functions |
      `----------*/

      ::RSA*
      RSA_priv2pub(::RSA* private_key)
      {
        ELLE_TRACE_FUNCTION(private_key);

        ELLE_ASSERT_NEQ(private_key, nullptr);

        ::BIO *mem = ::BIO_new(::BIO_s_mem());

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BIO(mem);

        if (::PEM_write_bio_RSAPublicKey(mem, private_key) != 1)
          throw Exception(
            elle::sprintf("unable to transcode the private key: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        ::RSA* public_key = nullptr;
        if ((public_key =
             ::PEM_read_bio_RSAPublicKey(mem, nullptr,
                                         nullptr, nullptr)) == nullptr)
          throw Exception(
            elle::sprintf("unable to extract the public key: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(mem);
        ::BIO_free(mem);

        return (public_key);
      }

      ::RSA*
      RSA_dup(::RSA* key)
      {
        ELLE_TRACE_FUNCTION(key);

        ELLE_ASSERT_NEQ(key, nullptr);

        // Increase the reference counter on this object rather
        // than duplicating the structure.
        ::RSA_up_ref(key);

        return (key);
      }
    }
  }
}

