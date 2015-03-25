#ifndef INFINIT_CRYPTOGRAPHY_RSA_PEM_HXX
# define INFINIT_CRYPTOGRAPHY_RSA_PEM_HXX

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

        template <typename T>
        T
        load_private(boost::filesystem::path const& path,
                   elle::String const& passphrase)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.pem");
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

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

        ::fclose(fd);

        return (T(key));
      }

      template <typename T>
      T
      load_public(boost::filesystem::path const& path)
                   elle::String const& passphrase)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.pem");
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

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

        ::fclose(fd);

        return (T(key));
      }

      template <typename T>
      T
      store(T const& object,
            boost::filesystem::path const& path,
            elle::String const& passphrase)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.pem");
        ELLE_TRACE_FUNCTION(path, passphrase);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        ::FILE* fd = nullptr;
      }
    }
  }
}

#endif
