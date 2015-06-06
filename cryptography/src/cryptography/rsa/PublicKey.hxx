#ifndef INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HXX
# define INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HXX

//
// ---------- Class -----------------------------------------------------------
//

# include <cryptography/Plain.hh>
# include <cryptography/serialization.hh>
# include <cryptography/hash.hh>

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
      Code
      PublicKey::encrypt(T const& value) const
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.rsa.PublicKey");
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("value: %x", value);

        elle::Buffer _value = cryptography::serialize(value);

        // XXX
        printf("ENCRYPT\n");
        _value.dump();

        return (Code(evp::asymmetric::encrypt(
                       _value,
                       this->_context.encrypt.get(),
                       ::EVP_PKEY_encrypt,
                       cipher::resolve(this->_envelope_cipher,
                                       this->_envelope_mode),
                       oneway::resolve(this->_digest_algorithm),
                       this->_context.envelope_padding_size)));
      }

      template <typename T>
      elle::Boolean
      PublicKey::verify(Signature const& signature,
                        T const& value) const
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.rsa.PublicKey");
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("signature: %x", signature);
        ELLE_DUMP("value: %x", value);

        static_assert(std::is_same<T, Digest>::value == false,
                      "this call should never have occured");

        elle::Buffer _value = cryptography::serialize(value);

        Digest digest = hash(Plain(_value), this->_digest_algorithm);

        return (this->verify(signature, digest));
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

ELLE_SERIALIZE_SPLIT(infinit::cryptography::rsa::PublicKey)

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::rsa::PublicKey,
                          archive,
                          value,
                          format)
{
  ELLE_ASSERT_NEQ(value._key, nullptr);

  elle::Buffer buffer =
    infinit::cryptography::rsa::der::encode_public(value._key->pkey.rsa);

  archive << buffer;
  archive << value._encryption_padding;
  archive << value._signature_padding;
  archive << value._digest_algorithm;
  archive << value._envelope_cipher;
  archive << value._envelope_mode;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::rsa::PublicKey,
                          archive,
                          value,
                          format)
{
  elle::Buffer buffer;

  archive >> buffer;
  archive >> value._encryption_padding;
  archive >> value._signature_padding;
  archive >> value._digest_algorithm;
  archive >> value._envelope_cipher;
  archive >> value._envelope_mode;

  ::RSA* rsa =
      infinit::cryptography::rsa::der::decode_public(buffer);

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
  struct hash<infinit::cryptography::rsa::PublicKey>
  {
    size_t
    operator ()(infinit::cryptography::rsa::PublicKey const& value) const
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
