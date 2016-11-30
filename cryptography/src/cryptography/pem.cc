#include <openssl/pem.h>
#include <openssl/err.h>

#include <elle/log.hh>

#include <cryptography/pem.hh>
#include <cryptography/Error.hh>
#include <cryptography/finally.hh>
#include <cryptography/cryptography.hh>

namespace infinit
{
  namespace cryptography
  {
    namespace pem
    {
      /*-----------------.
      | Static Functions |
      `-----------------*/

      /// The function emulating the passphrase callback.
      static
      int
      _callback(char* buffer, int size, int rwflag, void *u)
      {
        char* passphrase = (char*)u;
        int length = ::strlen(passphrase);

        // If too long, truncate.
        if (length > size)
          length = size;

        memcpy(buffer, passphrase, length);

        return (length);
      }

      /*----------.
      | Functions |
      `----------*/

      ::EVP_PKEY*
      import_public(boost::filesystem::path const& path)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        ::FILE* fd = nullptr;
        ::EVP_PKEY* key = nullptr;

        if ((fd = ::fopen(path.string().c_str(), "r")) == nullptr)
          throw Error(
            elle::sprintf("the PEM file '%s' does not seem to exist",
                          path.string()));

        if (::PEM_read_PUBKEY(
              fd,
              &key,
              nullptr,
              nullptr) == nullptr)
        {
          ::fclose(fd);

          throw Error(
            elle::sprintf("unable to read the public key from the PEM "
                          "file '%s'; please check the file format: %s",
                          path.string(),
                          ::ERR_error_string(ERR_get_error(),
                                             nullptr)));
        }

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

        ::fclose(fd);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(key);

        return (key);
      }

      ::EVP_PKEY*
      import_private(boost::filesystem::path const& path,
                     std::string const& passphrase)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        ::FILE* fd = nullptr;
        ::EVP_PKEY* key = nullptr;

        if ((fd = ::fopen(path.string().c_str(), "r")) == nullptr)
          throw Error(
            elle::sprintf("the PEM file '%s' does not seem to exist",
                          path.string()));

        if (::PEM_read_PrivateKey(
              fd,
              &key,
              &_callback,
              (void*)passphrase.c_str()) == nullptr)
        {
          ::fclose(fd);

          throw Error(
            elle::sprintf("unable to read the private key from the PEM "
                          "file '%s'; please check the file format or the "
                          "passphrase: %s",
                          path.string(),
                          ::ERR_error_string(ERR_get_error(),
                                             nullptr)));
        }

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

        ::fclose(fd);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(key);

        return (key);
      }

      void
      export_public(::EVP_PKEY* key,
                    boost::filesystem::path const& path)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        ::FILE* fd = nullptr;

        if ((fd = ::fopen(path.string().c_str(), "w")) == nullptr)
          throw Error(
            elle::sprintf("unable to open the file '%s'",
                          path.string()));

        if (::PEM_write_PUBKEY(fd,
                               key) != 1)
        {
          ::fclose(fd);

          throw Error(
            elle::sprintf("unable to write the public key to the PEM "
                          "file '%s'; please check the type of the key: %s",
                          path.string(),
                          ::ERR_error_string(ERR_get_error(),
                                             nullptr)));
        }

        ::fclose(fd);
      }

      void
      export_private(::EVP_PKEY* key,
                     boost::filesystem::path const& path,
                     std::string const& passphrase,
                     ::EVP_CIPHER const* cipher)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        ::FILE* fd = nullptr;

        if ((fd = ::fopen(path.string().c_str(), "w")) == nullptr)
          throw Error(
            elle::sprintf("unable to open the file '%s'",
                          path.string()));

        if (::PEM_write_PrivateKey(
              fd,
              key,
              cipher,
              (unsigned char*)passphrase.c_str(),
              (int)passphrase.length(),
              nullptr,
              nullptr) != 1)
        {
          ::fclose(fd);

          throw Error(
            elle::sprintf("unable to write the private key to the PEM "
                          "file '%s'; please check the type of the key: %s",
                          path.string(),
                          ::ERR_error_string(ERR_get_error(),
                                             nullptr)));
        }

        ::fclose(fd);
      }
    }
  }
}
