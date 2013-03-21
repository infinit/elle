#include <cryptography/rsa/keypair.hh>
#include <cryptography/Exception.hh>
#include <cryptography/finally.hh>

#include <elle/attribute.hh>
#include <elle/assert.hh>
#include <elle/log.hh>

#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/evp.h>

ELLE_LOG_COMPONENT("infinit.cryptography.rsa.keypair");

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace keypair
      {
        /*--------.
        | Classes |
        `--------*/

        // The key pair initializer.
        class Initializer
        {
        public:
          Initializer():
            _context(nullptr)
          {
            // Create the context for the RSA algorithm.
            if ((this->_context =
                 ::EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr)) == nullptr)
              throw Exception(::ERR_error_string(ERR_get_error(), nullptr));

            // Initialise the context for key generation.
            if (::EVP_PKEY_keygen_init(this->_context) <= 0)
              throw Exception(::ERR_error_string(ERR_get_error(), nullptr));
          }

          ~Initializer()
          {
            // Release the generation context.
            ::EVP_PKEY_CTX_free(this->_context);
          }

        private:
          ELLE_ATTRIBUTE_R(::EVP_PKEY_CTX*, context);
        };

        /*-----------------.
        | Static Functions |
        `-----------------*/

        /// Construct the key pair generation context.
        static
        ::EVP_PKEY_CTX*
        _context()
        {
          static Initializer initialized;

          ELLE_ASSERT(initialized.context() != nullptr);

          return (initialized.context());
        }

        /*----------.
        | Functions |
        `----------*/

        std::pair<PublicKey, PrivateKey>
        generate(elle::Natural32 const length)
        {
          ELLE_TRACE_FUNCTION(length);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Retrieve the key pair generation context.
          static ::EVP_PKEY_CTX* context = _context();

          // Set the key length in the keypair generation context.
          if (::EVP_PKEY_CTX_set_rsa_keygen_bits(context, length) <= 0)
            throw Exception(::ERR_error_string(ERR_get_error(), nullptr));

          ::EVP_PKEY* key = nullptr;

          // Generate the EVP key.
          if (::EVP_PKEY_keygen(context, &key) <= 0)
            throw Exception(::ERR_error_string(ERR_get_error(), nullptr));

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

          ELLE_ASSERT(key != nullptr);

          // Instanciate both a RSA public and private key based on the
          // EVP_PKEY.
          //
          // Note that some internal numbers need to be duplicated for the
          // instanciation of the public key while the private key is
          // constructed by transferring the ownership of the EVP_PKEY.
          PublicKey K(::BN_dup(key->pkey.rsa->n),
                      ::BN_dup(key->pkey.rsa->e));
          PrivateKey k(key);

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(key);

          return (std::pair<PublicKey, PrivateKey>(std::move(K), std::move(k)));
        }
      }
    }
  }
}
