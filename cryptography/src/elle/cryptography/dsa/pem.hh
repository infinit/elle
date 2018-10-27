#ifndef ELLE_CRYPTOGRAPHY_DSA_PEM_HH
# define ELLE_CRYPTOGRAPHY_DSA_PEM_HH

# include <elle/cryptography/dsa/PublicKey.hh>
# include <elle/cryptography/dsa/PrivateKey.hh>
# include <elle/cryptography/dsa/KeyPair.hh>
# include <elle/cryptography/dsa/defaults.hh>
# include <elle/cryptography/pem.hh>

# include <boost/filesystem.hpp>

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

        /// Import an DSA public key from a path.
        PublicKey
        import_K(boost::filesystem::path const& path,
                 Oneway const digest_algorithm =
                   defaults::digest_algorithm);
        /// Import an DSA private key from a path.
        PrivateKey
        import_k(boost::filesystem::path const& path,
                 std::string const& passphrase =
                   cryptography::pem::defaults::passphrase,
                 Oneway const digest_algorithm =
                   defaults::digest_algorithm);
        /// Import an DSA key pair from a path.
        KeyPair
        import_keypair(boost::filesystem::path const& path,
                       std::string const& passphrase =
                         cryptography::pem::defaults::passphrase,
                       Oneway const digest_algorithm =
                         defaults::digest_algorithm);
        /// Export an DSA public key.
        void
        export_K(PublicKey const& K,
                 boost::filesystem::path const& path);
        /// Export an DSA private key, providing the passphrase, cipher and
        /// mode to encrypt it with.
        void
        export_k(PrivateKey const& k,
                 boost::filesystem::path const& path,
                 std::string const& passphrase =
                   cryptography::pem::defaults::passphrase,
                 Cipher const& cipher =
                   cryptography::pem::defaults::cipher,
                 Mode const& mode =
                   cryptography::pem::defaults::mode);
        /// Export an DSA key pair.
        void
        export_keypair(KeyPair const& keypair,
                       boost::filesystem::path const& path,
                       std::string const& passphrase =
                         cryptography::pem::defaults::passphrase,
                       Cipher const& cipher =
                         cryptography::pem::defaults::cipher,
                       Mode const& mode =
                         cryptography::pem::defaults::mode);
      }
    }
  }
}

#endif
