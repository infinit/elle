#ifndef INFINIT_CRYPTOGRAPHY_SECRETKEY_HXX
# define INFINIT_CRYPTOGRAPHY_SECRETKEY_HXX

# include <elle/Buffer.hh>
# include <elle/log.hh>

# include <cryptography/Plain.hh>
# include <cryptography/serialization.hh>
# include <cryptography/evp.hh>

namespace infinit
{
  namespace cryptography
  {
    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    Code
    SecretKey::encrypt(T const& value) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.SecretKey");
      ELLE_DEBUG_FUNCTION(value);

      elle::Buffer archive = cryptography::serialize(value);

      ::EVP_CIPHER const* function_cipher =
          cipher::resolve(this->_cipher, this->_mode);
      ::EVP_MD const* function_oneway =
          oneway::resolve(this->_oneway);

      return (Code(evp::symmetric::encrypt(archive,
                                           this->_password,
                                           function_cipher,
                                           function_oneway)));
    }

    template <typename T>
    T
    SecretKey::decrypt(Code const& code) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.SecretKey");
      ELLE_DEBUG_FUNCTION(code);

      ::EVP_CIPHER const* function_cipher =
          cipher::resolve(this->_cipher, this->_mode);
      ::EVP_MD const* function_oneway =
          oneway::resolve(this->_oneway);

      Clear clear(evp::symmetric::decrypt(code.buffer(),
                                          this->_password,
                                          function_cipher,
                                          function_oneway));

      return (cryptography::deserialize<T>(clear.buffer()));
    }
  }
}

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::SecretKey,
                      archive,
                      value,
                      format)
{
  enforce(format == 0);

  archive & value._password;
  archive & value._cipher;
  archive & value._mode;
  archive & value._oneway;
}

#endif
