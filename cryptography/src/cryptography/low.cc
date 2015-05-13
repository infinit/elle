#include <cryptography/low.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/Exception.hh>

#include <elle/log.hh>
#include <elle/Buffer.hh>

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

        unsigned char* buffer = nullptr;
        int size = 0;

        if ((size = ::i2d_RSAPublicKey(private_key,
                                       &buffer)) <= 0)
          throw infinit::cryptography::Exception(
            elle::sprintf("unable to encode the RSA private key: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(buffer);

        const unsigned char* _buffer = buffer;

        ::RSA* public_key = nullptr;
        if ((public_key = ::d2i_RSAPublicKey(NULL,
                                             &_buffer,
                                             size)) == NULL)
          throw infinit::cryptography::Exception(
            elle::sprintf("unable to decode the RSA private key: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(public_key);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(buffer);
        ::OPENSSL_free(buffer);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(public_key);

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

      ::DSA*
      DSA_priv2pub(::DSA* private_key)
      {
        ELLE_TRACE_FUNCTION(private_key);

        ELLE_ASSERT_NEQ(private_key, nullptr);

        unsigned char* buffer = nullptr;
        int size = 0;

        if ((size = ::i2d_DSAPublicKey(private_key,
                                       &buffer)) <= 0)
          throw infinit::cryptography::Exception(
            elle::sprintf("unable to encode the DSA private key: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(buffer);

        const unsigned char* _buffer = buffer;

        ::DSA* public_key = nullptr;
        if ((public_key = ::d2i_DSAPublicKey(NULL,
                                             &_buffer,
                                             size)) == NULL)
          throw infinit::cryptography::Exception(
            elle::sprintf("unable to decode the DSA private key: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_DSA(public_key);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(buffer);
        ::OPENSSL_free(buffer);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(public_key);

        return (public_key);
      }

      ::DSA*
      DSA_dup(::DSA* key)
      {
        ELLE_TRACE_FUNCTION(key);

        ELLE_ASSERT_NEQ(key, nullptr);

        // Increase the reference counter on this object rather
        // than duplicating the structure.
        ::DSA_up_ref(key);

        return (key);
      }
    }
  }
}

