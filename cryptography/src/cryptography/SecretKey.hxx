#ifndef INFINIT_CRYPTOGRAPHY_SECRETKEY_HXX
# define INFINIT_CRYPTOGRAPHY_SECRETKEY_HXX

# include <elle/Buffer.hh>
# include <elle/log.hh>

# include <cryptography/Plain.hh>
# include <cryptography/serialization.hh>
# include <cryptography/evp.hh>

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
/*-------.
| Legacy |
`-------*/

namespace infinit
{
  namespace cryptography
  {
    template <typename T>
    Code
    SecretKey::encrypt(T const& value) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.SecretKey");
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("value: %x", value);

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
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("code: %x", code);

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

#  include <elle/serialize/Serializer.hh>
#  include <elle/serialize/StaticFormat.hh>

#  include <cryptography/Exception.hh>

ELLE_SERIALIZE_STATIC_FORMAT(infinit::cryptography::SecretKey, 1);

ELLE_SERIALIZE_SPLIT(infinit::cryptography::SecretKey);

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::SecretKey,
                          archive,
                          value,
                          format)
{
  switch (format)
  {
    case 0:
    {
      if (value._cipher == infinit::cryptography::Cipher::null)
        throw infinit::cryptography::Exception(
          elle::sprintf("unable to serialize a null cipher in an old format: "
                        "%s", format));

      elle::Natural16 _cipher =
        static_cast<elle::Natural16>(value._cipher) - 1;

      archive << _cipher;
      archive << value._password;
      archive << value._oneway;

      if (value._mode != infinit::cryptography::Mode::cbc)
        throw infinit::cryptography::Exception(
          elle::sprintf("unable to serialize a non-CBC mode in an old format: "
                        "%s", format));

      break;
    }
    case 1:
    {
      archive << value._password;
      archive << value._cipher;
      archive << value._mode;
      archive << value._oneway;

      break;
    }
    default:
      throw infinit::cryptography::Exception(
        elle::sprintf("unknown format '%s'", format));
  }
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::SecretKey,
                          archive,
                          value,
                          format)
{
  switch (format)
  {
    case 0:
    {
      elle::Natural16 _cipher;
      archive >> _cipher;
      // This comes from the fact that the Cipher enumeration
      // now includes a null value at the very beginning i.e value 0.
      value._cipher = static_cast<infinit::cryptography::Cipher>(_cipher + 1);

      archive >> value._password;
      archive >> value._oneway;

      value._mode = infinit::cryptography::Mode::cbc;

      break;
    }
    case 1:
    {
      archive >> value._password;
      archive >> value._cipher;
      archive >> value._mode;
      archive >> value._oneway;

      break;
    }
    default:
      throw infinit::cryptography::Exception(
        elle::sprintf("unknown format '%s'", format));
  }
}
# endif

#endif
