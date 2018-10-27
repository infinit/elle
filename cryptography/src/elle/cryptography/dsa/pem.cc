#include <openssl/pem.h>
#include <openssl/err.h>

#include <elle/log.hh>

#include <elle/cryptography/cryptography.hh>
#include <elle/cryptography/dsa/pem.hh>
#include <elle/cryptography/finally.hh>
#include <elle/cryptography/pem.hh>

namespace elle
{
  namespace cryptography
  {
    namespace dsa
    {
      namespace pem
      {
        /*----------.
        | Functions |
        `----------*/

        PublicKey
        import_K(boost::filesystem::path const& path,
                 Oneway const digest_algorithm)
        {
          ::EVP_PKEY* key = cryptography::pem::import_public(path);

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

          PublicKey K(key,
                      digest_algorithm);

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(key);

          return (K);
        }

        PrivateKey
        import_k(boost::filesystem::path const& path,
                 std::string const& passphrase,
                 Oneway const digest_algorithm)
        {
          ::EVP_PKEY* key = cryptography::pem::import_private(path,
                                                              passphrase);

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

          PrivateKey k(key,
                       digest_algorithm);

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(key);

          return (k);
        }

        KeyPair
        import_keypair(boost::filesystem::path const& path,
                       std::string const& passphrase,
                       Oneway const digest_algorithm)
        {
          PrivateKey k = import_k(path, passphrase,
                                  digest_algorithm);

          PublicKey K(k);

          return (KeyPair(std::move(K), std::move(k)));
        }

        void
        export_K(PublicKey const& K,
                 boost::filesystem::path const& path)
        {
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
