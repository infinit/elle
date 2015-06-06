#ifndef INFINIT_CRYPTOGRAPHY_DSA_PRIVATEKEY_HXX
# define INFINIT_CRYPTOGRAPHY_DSA_PRIVATEKEY_HXX

//
// ---------- Class -----------------------------------------------------------
//

# include <cryptography/serialization.hh>
# include <cryptography/hash.hh>
# include <cryptography/Plain.hh>
# include <cryptography/Digest.hh>

# include <elle/Buffer.hh>
# include <elle/log.hh>

namespace infinit
{
  namespace cryptography
  {
    namespace dsa
    {
      /*--------.
      | Methods |
      `--------*/

      template <typename T>
      Signature
      PrivateKey::sign(T const& value) const
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.dsa.PrivateKey");
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("value: %x", value);

        static_assert(std::is_same<T, Digest>::value == false,
                      "this call should never have occured");

        elle::Buffer _value = cryptography::serialize(value);

        Digest digest = hash(Plain(_value), this->_digest_algorithm);

        return (this->sign(digest));
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
# include <cryptography/dsa/der.hh>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::dsa::PrivateKey)

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::dsa::PrivateKey,
                          archive,
                          value,
                          format)
{
  ELLE_ASSERT_NEQ(value._key, nullptr);

  elle::Buffer buffer =
    infinit::cryptography::dsa::der::encode_private(value._key->pkey.dsa);

  archive << buffer;
  archive << value._digest_algorithm;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::dsa::PrivateKey,
                          archive,
                          value,
                          format)
{
  elle::Buffer buffer;

  archive >> buffer;
  archive >> value._digest_algorithm;

  ::DSA* dsa =
      infinit::cryptography::dsa::der::decode_private(buffer);

  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_DSA(dsa);

  value._construct(dsa);

  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(dsa);

  value._prepare();

  value._check();
}

//
// ---------- Hash ------------------------------------------------------------
//

namespace std
{
  template <>
  struct hash<infinit::cryptography::dsa::PrivateKey>
  {
    size_t
    operator ()(infinit::cryptography::dsa::PrivateKey const& value) const
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
