#ifndef INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HXX
# define INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HXX

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)

//
// ---------- Class -----------------------------------------------------------
//

#  include <cryptography/Plain.hh>
#  include <cryptography/serialization.hh>
#  include <cryptography/hash.hh>
#  include <cryptography/envelope.hh>

#  include <elle/Buffer.hh>
#  include <elle/log.hh>

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

        return (Code(envelope::seal(_value,
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

#  include <elle/serialize/Serializer.hh>
#  include <elle/serialize/StaticFormat.hh>

#  include <cryptography/finally.hh>
#  include <cryptography/Cryptosystem.hh>
#  include <cryptography/bn.hh>
#  include <cryptography/Exception.hh>
#  include <cryptography/Cipher.hh>
#  include <cryptography/rsa/der.hh>
#  include <cryptography/rsa/Padding.hh>
#  include <cryptography/rsa/legacy.hh>

ELLE_SERIALIZE_STATIC_FORMAT(infinit::cryptography::rsa::PublicKey, 2);

ELLE_SERIALIZE_SPLIT(infinit::cryptography::rsa::PublicKey);

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::rsa::PublicKey,
                          archive,
                          value,
                          format)
{
  switch (format)
  {
    case 0:
    case 1:
    {
      infinit::cryptography::Cryptosystem cryptosystem =
        infinit::cryptography::Cryptosystem::rsa;
      archive << cryptosystem;

      infinit::cryptography::rsa::legacy::Dummy implementation;
      archive << implementation;

      archive << *value._key->pkey.rsa->n
              << *value._key->pkey.rsa->e;

      break;
    }
    case 2:
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

      break;
    }
    default:
      throw infinit::cryptography::Exception(
        elle::sprintf("unknown format '%s'", format));
  }
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::rsa::PublicKey,
                          archive,
                          value,
                          format)
{
  switch (format)
  {
    case 0:
    case 1:
    {
      // Extract the cryptosystem.
      infinit::cryptography::Cryptosystem cryptosystem;
      archive >> cryptosystem;

      // Emulate deserializing a subclass.
      infinit::cryptography::rsa::legacy::Dummy implementation;
      archive >> implementation;

      // Extract the big numbers.
      ::BIGNUM *n = ::BN_new();
      ::BIGNUM *e = ::BN_new();

      ELLE_ASSERT_NEQ(n, nullptr);
      ELLE_ASSERT_NEQ(e, nullptr);

      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(n);
      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(e);

      archive >> *n
              >> *e;

      // Build the RSA object.
      ::RSA* rsa = ::RSA_new();

      ELLE_ASSERT_NEQ(rsa, nullptr);

      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);

      rsa->n = n;
      rsa->e = e;

      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(n);
      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(e);

      // Set the default configuration values.
      switch (format)
      {
        case 0:
        {
          value._encryption_padding =
            infinit::cryptography::rsa::Padding::oaep;
          value._signature_padding =
            infinit::cryptography::rsa::Padding::pkcs1;
          value._digest_algorithm =
            infinit::cryptography::Oneway::sha256;
          value._envelope_cipher =
            infinit::cryptography::Cipher::aes256;
          value._envelope_mode =
            infinit::cryptography::Mode::cbc;

          break;
        }
        case 1:
        {
          value._encryption_padding =
            infinit::cryptography::rsa::Padding::pkcs1;
          value._signature_padding =
            infinit::cryptography::rsa::Padding::pkcs1;
          value._digest_algorithm =
            infinit::cryptography::Oneway::sha256;
          value._envelope_cipher =
            infinit::cryptography::Cipher::aes256;
          value._envelope_mode =
            infinit::cryptography::Mode::cbc;

          break;
        }
        default:
          unreachable();
      }

      // Construct and prepare the final object.
      value._construct(rsa);

      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);

      value._prepare();

      // Specifically set the signature digest method to null
      // because it was not set in the versions 0 and 1.
      if (::EVP_PKEY_CTX_set_signature_md(value._context.verify.get(),
                                          nullptr) <= 0)
        throw Exception(
          elle::sprintf("unable to set the EVP_PKEY context's digest "
                        "function: %s",
                        ::ERR_error_string(ERR_get_error(), nullptr)));

      // Validate the key.
      value._check();

      break;
    }
    case 2:
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

      break;
    }
    default:
      throw infinit::cryptography::Exception(
        elle::sprintf("unknown format '%s'", format));
  }
}

# endif

//
// ---------- Hash ------------------------------------------------------------
//

# include <elle/serialization/binary.hh>

namespace std
{
  template <>
  struct hash<infinit::cryptography::rsa::PublicKey>
  {
    size_t
    operator ()(infinit::cryptography::rsa::PublicKey const& value) const
    {
      std::stringstream stream;
      {
        elle::serialization::binary::SerializerOut output(stream, false); // XXX
        output.serialize("value", value);
      }

      size_t result = std::hash<std::string>()(stream.str());

      return (result);
    }
  };
}

#endif
