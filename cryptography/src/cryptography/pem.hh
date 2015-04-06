#ifndef INFINIT_CRYPTOGRAPHY_PEM_HH
# define INFINIT_CRYPTOGRAPHY_PEM_HH

# include <openssl/evp.h>

# include <boost/filesystem.hpp>

# include <elle/types.hh>

namespace infinit
{
  namespace cryptography
  {
    /// These functions should not be used directly. Prefer the
    /// cryptosystem-specific PEM functions e.g rsa::pem::*.
    namespace pem
    {
      /*----------.
      | Functions |
      `----------*/

      /// Import an EVP public key from a path.
      ::EVP_PKEY*
      import_public(boost::filesystem::path const& path);
      /// Import an EVP private key from a path, decrypting the
      /// key with the provided passphrase.
      ::EVP_PKEY*
      import_private(boost::filesystem::path const& path,
                     elle::String const& passphrase);
      /// Export an EVP public key.
      void
      export_public(::EVP_PKEY* key,
                    boost::filesystem::path const& path);
      /// Export an EVP private key, providing a passphrase and the cipher
      /// function to use to encrypt the key with..
      ///
      /// WARNING: Not all cipher/mode combinations are supported. For instance
      ///          one cannot encrypt the private key with a DESX/CBC cipher
      ///          mode.
      void
      export_private(::EVP_PKEY* key,
                     boost::filesystem::path const& path,
                     elle::String const& passphrase,
                     ::EVP_CIPHER const* cipher);
    }
  }
}

#endif
