#include <cryptography/rsa/keypair.hh>
#include <cryptography/rsa/Seed.hh>
#include <cryptography/Exception.hh>
#include <cryptography/Seed.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/deleter.hh>
#include <cryptography/types.hh>

#include <elle/attribute.hh>
#include <elle/assert.hh>
#include <elle/log.hh>

#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
# include <dopenssl/rsa.hh>
#endif

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
          Initializer()
          {
            // Create the context for the RSA algorithm.
            this->_context.reset(
              ::EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr));

            if (this->_context == nullptr)
              throw Exception(
                elle::sprintf("unable to allocate a keypair generation "
                              "context: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            // Initialise the context for key generation.
            if (::EVP_PKEY_keygen_init(this->_context.get()) <= 0)
              throw Exception(
                elle::sprintf("unable to initialize the keypair generation "
                              "context: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));
          }

        private:
          ELLE_ATTRIBUTE_R(types::EVP_PKEY_CTX, context);
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

          ELLE_ASSERT_NEQ(initialized.context(), nullptr);

          return (initialized.context().get());
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
            throw Exception(
              elle::sprintf("unable to set the length of the keypair to "
                            "be generated: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ::EVP_PKEY* key = nullptr;

          // Generate the EVP key.
          if (::EVP_PKEY_keygen(context, &key) <= 0)
            throw Exception(
              elle::sprintf("unable to generate a keypair: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

          ELLE_ASSERT_NEQ(key, nullptr);

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

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        std::pair<PublicKey, PrivateKey>
        deduce(cryptography::seed::Interface const& seed)
        {
          ELLE_TRACE_FUNCTION("");
          ELLE_DUMP("seed: %x", seed);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          ELLE_ASSERT_EQ(seed.cryptosystem(), Cryptosystem::rsa);

          // Cast the interface into an actual RSA seed.
          ELLE_ASSERT_NEQ(dynamic_cast<Seed const*>(&seed), nullptr);
          Seed const& _seed = static_cast<Seed const&>(seed);

          // Deduce the RSA key from the given seed.
          ::RSA* rsa = nullptr;

          if ((rsa = ::dRSA_deduce_privatekey(
                 _seed.length(),
                 static_cast<unsigned char const*>(_seed.buffer().contents()),
                 _seed.buffer().size())) == nullptr)
            throw Exception(
              elle::sprintf("unable to deduce the RSA key from the given "
                            "seed: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);

          // Instanciate both a RSA public and private key based on the RSA
          // structure.
          //
          // Note that some internal numbers need to be duplicated for the
          // instanciation of the public key while the private key is
          // constructed by transferring the ownership of the RSA.
          PublicKey K(::BN_dup(rsa->n),
                      ::BN_dup(rsa->e));
          PrivateKey k(rsa);

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);

          return (std::pair<PublicKey, PrivateKey>(std::move(K), std::move(k)));
        }
#endif

        std::pair<PublicKey, PrivateKey>
        construct(::EVP_PKEY* key)
        {
          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

          // Instanciate both a RSA public and private key based on the EVP's
          // RSA structure.
          //
          // Note that some internal numbers need to be duplicated for the
          // instanciation of the public key while the private key is
          // constructed by transferring the ownership of the RSA.
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
