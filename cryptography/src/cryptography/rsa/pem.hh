#ifndef INFINIT_CRYPTOGRAPHY_RSA_PEM_HH
# define INFINIT_CRYPTOGRAPHY_RSA_PEM_HH

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

      /* XXX

The old PrivateKey write routines are retained for compatibility. New applications should write private keys using the PEM_write_bio_PKCS8PrivateKey() or PEM_write_PKCS8PrivateKey() routines because they are more secure (they use an iteration count of 2048 whereas the traditional routines use a count of 1) unless compatibility with older versions of OpenSSL is important.

The PrivateKey read routines can be used in all applications because they handle all formats transparently.

A frequent cause of problems is attempting to use the PEM routines like this:

 X509 *x;
 PEM_read_bio_X509(bp, &x, 0, NULL);
this is a bug because an attempt will be made to reuse the data at x which is an uninitialised pointer.

      */

      /// A helper for easing the process of loading a key pair, a public or
      /// private key from a file given its final type.
      ///
      /// Note that a passphrase is required for loading a key pair or private
      /// key while loading a public key does not require anything more than
      /// the path to the PEM file.
      template <typename T>
      T
      load_public(boost::filesystem::path const& path,
                  elle::String const& passphrase = "");
      /// Store a public, private or pair of key(s) in a file.
      template <typename T>
      void
      store(T const& object,
            boost::filesystem::path const& path,
            elle::String const& passphrase = "");
      // The function emulating the passphrase callback.
      int
      callback(char* buffer,
               int size,
               int rwflag,
               void *u);
    }
  }
}

// XXX # include <cryptography/pem.hxx>

#endif
