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

# include <cryptography/finally.hh>
# include <cryptography/rsa/der.hh>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::rsa::PrivateKey)

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::rsa::PrivateKey,
                          archive,
                          value,
                          format)
{
  ELLE_ASSERT_NEQ(value._key, nullptr);

  elle::Buffer buffer =
    infinit::cryptography::rsa::der::encode_private(value._key->pkey.rsa);

  archive << buffer;
  archive << value._encryption_padding;
  archive << value._signature_padding;
  archive << value._digest_algorithm;
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

  ::RSA* rsa =
      infinit::cryptography::rsa::der::decode_private(buffer);

  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);

  value._construct(rsa);

  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);

  value._prepare();

  value._check();
}

//
// ---------- Hash ------------------------------------------------------------
//

namespace std
{
  template <>
  struct hash<infinit::cryptography::rsa::PrivateKey>
  {
    size_t
    operator ()(infinit::cryptography::rsa::PrivateKey const& value) const
    {
      std::stringstream stream;
      elle::serialize::OutputBinaryArchive archive(stream);

      archive << value;

      size_t result = std::hash<std::string>()(stream.str());

      return (result);
    }
  };
}

#endif
