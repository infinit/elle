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

        return (this->sign(Plain(elle::WeakBuffer(buffer))));
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

  // XXX[use a WeakBuffer to avoid a copy]
  //elle::ConstWeakBuffer buffer(_buffer, _size);
  //archive << buffer;
  // XXX
  elle::Buffer buffer(_buffer, _size);
  archive << buffer;

  ::OPENSSL_free(_buffer);
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::rsa::PrivateKey,
                          archive,
                          value,
                          format)
{
  elle::Buffer buffer;
  archive >> buffer;

  const unsigned char* _buffer = buffer.contents();
  long _size = buffer.size();

  ::RSA* rsa = nullptr;
  if ((rsa = ::d2i_RSAPrivateKey(NULL, &_buffer, _size)) == NULL)
    throw infinit::cryptography::Exception(
      elle::sprintf("unable to decode the RSA private key: %s",
                    ::ERR_error_string(ERR_get_error(), nullptr)));

  value._construct(rsa);
  value._prepare();

  ELLE_ASSERT_NEQ(value._key, nullptr);
}

#endif
