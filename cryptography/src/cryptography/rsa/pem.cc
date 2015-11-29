#include <openssl/pem.h>
#include <openssl/err.h>

#include <elle/log.hh>

#include <cryptography/rsa/pem.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/Error.hh>
#include <cryptography/finally.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/pem.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.rsa.pem");

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace pem
      {
        /*----------.
        | Functions |
        `----------*/

        PublicKey
        import_K(boost::filesystem::path const& path)
        {
          ELLE_TRACE_FUNCTION(path);

          ::EVP_PKEY* key = cryptography::pem::import_public(path);

          if (key->type != EVP_PKEY_RSA)
            throw Error(
              elle::sprintf("the key type is not RSA: %s", key->type));

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

          PublicKey K(key);

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(key);

          return (K);
        }

        PrivateKey
        import_k(boost::filesystem::path const& path,
                 std::string const& passphrase)
        {
          ELLE_TRACE_FUNCTION(path, passphrase);

          ::EVP_PKEY* key = cryptography::pem::import_private(path,
                                                              passphrase);

          if (key->type != EVP_PKEY_RSA)
            throw Error(
              elle::sprintf("the key type is not RSA: %s", key->type));

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

          PrivateKey k(key);

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(key);

          return (k);
        }

        KeyPair
        import_keypair(boost::filesystem::path const& path,
                       std::string const& passphrase)
        {
          ELLE_TRACE_FUNCTION(path, passphrase);

          PrivateKey k = import_k(path, passphrase);
          PublicKey K(k);

          return (KeyPair(std::move(K), std::move(k)));
        }

        void
        export_K(PublicKey const& K,
                 boost::filesystem::path const& path)
        {
          ELLE_TRACE_FUNCTION(K, path);

          cryptography::pem::export_public(K.key().get(),
                                           path);
        }

        void
        export_k(PrivateKey const& k,
                 boost::filesystem::path const& path,
                 std::string const& passphrase,
                 Cipher const& cipher,
                 Mode const& mode)
        {
          ELLE_TRACE_FUNCTION(k, path, passphrase, cipher, mode);

          cryptography::pem::export_private(k.key().get(),
                                            path,
                                            passphrase,
                                            cipher::resolve(cipher, mode));
        }

        void
        export_keypair(KeyPair const& keypair,
                       boost::filesystem::path const& path,
                       std::string const& passphrase,
                       Cipher const& cipher,
                       Mode const& mode)
        {
          ELLE_TRACE_FUNCTION(keypair, path, passphrase, cipher, mode);

          export_k(keypair.k(),
                   path,
                   passphrase,
                   cipher,
                   mode);
        }
      }
    }
  }
}
