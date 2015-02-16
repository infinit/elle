#ifndef INFINIT_CRYPTOGRAPHY_PEM_HH
# define INFINIT_CRYPTOGRAPHY_PEM_HH

# include <cryptography/KeyPair.hh>
# include <cryptography/PublicKey.hh>
# include <cryptography/PrivateKey.hh>

# include <boost/filesystem/path.hpp>

namespace infinit
{
  namespace cryptography
  {
    /// Provide functions for manipulating Privacy Enhanced Mail (PEM) files.
    namespace pem
    {
      /*----------.
      | Functions |
      `----------*/

      /// A helper for easing the process of loading a key pair, a public or
      /// private key from a file.
      ///
      /// Note that a passphrase is required for loading a key pair or private
      /// key while loading a public key does not require anything more than
      /// the path to the PEM file.
      template <typename T>
      T
      load(boost::filesystem::path const& path,
           elle::String const& passphrase);
      // XXX
      /// Store a public, private or pair of key(s) to a file.
      template <typename T>
      void
      store(T const& object,
            boost::filesystem::path const& path);
      /// Load a key pair from a file.
      KeyPair
      load_keypair(boost::filesystem::path const& path,
                   elle::String const& passphrase);
      /// Load a public key from a file.
      PublicKey
      load_K(boost::filesystem::path const& path,
             elle::String const& passphrase = "");
      /// Load a private key from a file.
      PrivateKey
      load_k(boost::filesystem::path const& path,
             elle::String const& passphrase);
    }
  }
}

# include <cryptography/pem.hxx>

#endif
