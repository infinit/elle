#include <cryptography/pem.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/rsa/KeyPair.hh>

#include <elle/log.hh>

#include <openssl/evp.h>
#include <openssl/pem.h>

#include <cstdio>

ELLE_LOG_COMPONENT("infinit.cryptography.pem");

namespace infinit
{
  namespace cryptography
  {
    namespace pem
    {
      /*----------.
      | Functions |
      `----------*/

      // The function emulating the passphrase callback.
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

      KeyPair
      load_keypair(boost::filesystem::path const& path,
                   elle::String const& passphrase)
      {
        ELLE_TRACE_FUNCTION(path, passphrase);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        ::FILE* fd = nullptr;
        ::EVP_PKEY* key = nullptr;

        if ((fd = ::fopen(path.string().c_str(), "r")) == nullptr)
          throw Exception(
            elle::sprintf("the PEM file '%s' does not seem to exist",
                          path.string()));

        if (::PEM_read_PrivateKey(
              fd,
              &key,
              &_callback,
              (void*)passphrase.c_str()) == nullptr)
        {
          ::fclose(fd);

          throw Exception(
            elle::sprintf("unable to read the private key from the PEM "
                          "file; please check the file format or the "
                          "passphrase: %s",
                          ::ERR_error_string(ERR_get_error(),
                                             nullptr)));
        }

        ::fclose(fd);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

        int type = ::EVP_PKEY_type(key->type);

        switch (type)
        {
          case EVP_PKEY_RSA:
          {
            INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(key);

            std::pair<rsa::PublicKey, rsa::PrivateKey> pair =
              rsa::keypair::construct(key);

            // Construct high-level public and private keys.
            std::unique_ptr<publickey::Interface> K(
              new rsa::PublicKey(std::move(pair.first)));
            std::unique_ptr<privatekey::Interface> k(
              new rsa::PrivateKey(std::move(pair.second)));

            // Construct a key pair based on both public and private key.
            return (KeyPair(PublicKey(std::move(K)), PrivateKey(std::move(k))));
          }
          case EVP_PKEY_DSA:
          case EVP_PKEY_DH:
          case EVP_PKEY_EC:
            throw Exception(
              elle::sprintf("unsupported cryptosystem; %s", type));
          case NID_undef:
          default:
            throw Exception(
              elle::sprintf("unknown cryptosystem type: %s", type));
        }

        elle::unreachable();
      }

      PublicKey
      load_K(boost::filesystem::path const& path,
             elle::String const& passphrase)
      {
        ELLE_TRACE_FUNCTION(path, passphrase);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        ::FILE* fd = nullptr;
        ::EVP_PKEY* key = nullptr;

        if ((fd = ::fopen(path.string().c_str(), "r")) == nullptr)
          throw Exception(
            elle::sprintf("the PEM file '%s' does not seem to exist",
                          path.string()));

        if (::PEM_read_PUBKEY(
              fd,
              &key,
              &_callback,
              (void*)passphrase.c_str()) == nullptr)
        {
          ::fclose(fd);

          throw Exception(
            elle::sprintf("unable to read the public key from the PEM "
                          "file; please check the file format or the "
                          "passphrase: %s",
                          ::ERR_error_string(ERR_get_error(),
                                             nullptr)));
        }

        ::fclose(fd);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

        int type = ::EVP_PKEY_type(key->type);

        switch (type)
        {
          case EVP_PKEY_RSA:
          {
            INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(key);

            rsa::PublicKey implementation = rsa::publickey::construct(key);

            // Construct a high-level public key.
            std::unique_ptr<publickey::Interface> K(
              new rsa::PublicKey(std::move(implementation)));

            // Construct a public key based on the interface.
            return (PublicKey(std::move(K)));
          }
          case EVP_PKEY_DSA:
          case EVP_PKEY_DH:
          case EVP_PKEY_EC:
            throw Exception(
              elle::sprintf("unsupported cryptosystem; %s", type));
          case NID_undef:
          default:
            throw Exception(
              elle::sprintf("unknown cryptosystem type: %s", type));
        }

        elle::unreachable();
      }

      PrivateKey
      load_k(boost::filesystem::path const& path,
             elle::String const& passphrase)
      {
        cryptography::KeyPair keypair = load_keypair(path, passphrase);

        return (keypair.k());
      }
    }
  }
}
