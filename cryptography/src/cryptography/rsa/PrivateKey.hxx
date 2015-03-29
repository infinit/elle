#ifndef INFINIT_CRYPTOGRAPHY_RSA_PRIVATEKEY_HXX
# define INFINIT_CRYPTOGRAPHY_RSA_PRIVATEKEY_HXX

//
// ---------- Class -----------------------------------------------------------
//

# include <elle/Buffer.hh>
# include <elle/log.hh>

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /*--------.
      | Methods |
      `--------*/

      template <typename T>
      T
      PrivateKey::decrypt(Code const& code) const
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.rsa.PrivateKey");
        ELLE_DEBUG_FUNCTION(code);

        static_assert(std::is_same<T, Clear>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::Buffer>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::WeakBuffer>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::ConstWeakBuffer>::value == false,
                      "this call should never have occured");

        Clear clear(this->decrypt(code));

        // XXX[this is the way it should be] T value(clear.buffer().reader());
        T value;
        clear.buffer().reader() >> value;

        return (value);
      }

      template <typename T>
      Signature
      PrivateKey::sign(T const& value) const
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.rsa.PrivateKey");
        ELLE_DEBUG_FUNCTION(value);

        static_assert(std::is_same<T, Digest>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, Plain>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::Buffer>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::WeakBuffer>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::ConstWeakBuffer>::value == false,
                      "this call should never have occured");

        elle::Buffer buffer;
        buffer.writer() << value;

        return (this->sign(Plain(buffer)));
      }
    }
  }
}

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>
# include <elle/serialize/StaticFormat.hh>

# include <cryptography/Exception.hh>
# include <cryptography/finally.hh>
# include <cryptography/bn.hh>

# include <openssl/rsa.h>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::rsa::PrivateKey)

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::rsa::PrivateKey,
                          archive,
                          value,
                          format)
{
  ELLE_ASSERT_NEQ(value._key, nullptr);

  unsigned char* _buffer = nullptr;
  int _size = ::i2d_RSAPrivateKey(value._key->pkey.rsa, &_buffer);
  if (_size <= 0)
    throw infinit::cryptography::Exception(
      elle::sprintf("unable to encode the RSA private key: %s",
                    ::ERR_error_string(ERR_get_error(), nullptr)));

  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(_buffer);

  // XXX[use a WeakBuffer to avoid a copy: not supported by Raph's
  //     serialization]
  //elle::ConstWeakBuffer buffer(_buffer, _size);
  //archive << buffer;
  // XXX
  elle::Buffer buffer(_buffer, _size);
  archive << buffer;
  archive << value._encryption_padding;
  archive << value._signature_padding;
  archive << value._digest_algorithm;

  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(_buffer);
  ::OPENSSL_free(_buffer);
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::rsa::PrivateKey,
                          archive,
                          value,
                          format)
{
  elle::Buffer buffer;
  archive >> buffer;
  archive >> value._encryption_padding;
  archive >> value._signature_padding;
  archive >> value._digest_algorithm;

  const unsigned char* _buffer = buffer.contents();
  long _size = buffer.size();

  ::RSA* rsa = nullptr;
  if ((rsa = ::d2i_RSAPrivateKey(NULL, &_buffer, _size)) == NULL)
    throw infinit::cryptography::Exception(
      elle::sprintf("unable to decode the RSA private key: %s",
                    ::ERR_error_string(ERR_get_error(), nullptr)));

  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);

  value._construct(rsa);

  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);

  value._prepare();

  ELLE_ASSERT_NEQ(value._key, nullptr);
}

//
// ---------- Hash ------------------------------------------------------------
//

#include <exception>

namespace std
{
  template <>
  struct hash<infinit::cryptography::rsa::PrivateKey>
  {
    size_t
    operator ()(infinit::cryptography::rsa::PrivateKey const& value) const
    {
      unsigned char* buffer = nullptr;
      int size = ::i2d_RSAPrivateKey(value.key()->pkey.rsa, &buffer);

      if (size <= 0)
        throw std::runtime_error(
          elle::sprintf("unable to encode the RSA private key: %s",
                        ::ERR_error_string(ERR_get_error(), nullptr)));

      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(buffer);

      std::string string(reinterpret_cast<char const*>(buffer), size);

      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(buffer);
      ::OPENSSL_free(buffer);

      size_t result = std::hash<std::string>()(string);

      return (result);
    }
  };
}

#endif
