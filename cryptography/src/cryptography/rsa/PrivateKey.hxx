#ifndef INFINIT_CRYPTOGRAPHY_RSA_PRIVATEKEY_HXX
# define INFINIT_CRYPTOGRAPHY_RSA_PRIVATEKEY_HXX

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)

//
// ---------- Class -----------------------------------------------------------
//

#  include <cryptography/serialization.hh>
#  include <cryptography/Cryptosystem.hh>
#  include <cryptography/Plain.hh>
#  include <cryptography/Exception.hh>
#  include <cryptography/hash.hh>
#  include <cryptography/evp.hh>

#  include <elle/Buffer.hh>
#  include <elle/log.hh>

#  include <openssl/err.h>

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
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("code: %x", code);

        Clear clear(evp::asymmetric::decrypt(code.buffer(),
                                             this->_context.decrypt.get(),
                                             ::EVP_PKEY_decrypt));

        return (cryptography::deserialize<T>(clear.buffer()));
      }

      template <typename T>
      Signature
      PrivateKey::sign(T const& value) const
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.rsa.PrivateKey");
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

#  include <elle/serialize/Serializer.hh>
#  include <elle/serialize/StaticFormat.hh>

#  include <cryptography/finally.hh>
#  include <cryptography/rsa/der.hh>
#  include <cryptography/rsa/legacy.hh>

ELLE_SERIALIZE_STATIC_FORMAT(infinit::cryptography::rsa::PrivateKey, 2);

ELLE_SERIALIZE_SPLIT(infinit::cryptography::rsa::PrivateKey)

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::rsa::PrivateKey,
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

      infinit::cryptography::legacy::Dummy implementation;
      archive << implementation;

      archive << *value._key->pkey.rsa->n
              << *value._key->pkey.rsa->e
              << *value._key->pkey.rsa->d
              << *value._key->pkey.rsa->p
              << *value._key->pkey.rsa->q
              << *value._key->pkey.rsa->dmp1
              << *value._key->pkey.rsa->dmq1
              << *value._key->pkey.rsa->iqmp;

      break;
    }
    case 2:
    {
      ELLE_ASSERT_NEQ(value._key, nullptr);

      elle::Buffer buffer =
        infinit::cryptography::rsa::der::encode_private(value._key->pkey.rsa);

      archive << buffer;
      archive << value._encryption_padding;
      archive << value._signature_padding;
      archive << value._digest_algorithm;

      break;
    }
    default:
      throw infinit::cryptography::Exception(
        elle::sprintf("unknown format '%s'", format));
  }
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::rsa::PrivateKey,
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
      infinit::cryptography::legacy::Dummy implementation;
      archive >> implementation;

      // Extract the big numbers.
      ::BIGNUM *n = ::BN_new();
      ::BIGNUM *e = ::BN_new();
      ::BIGNUM *d = ::BN_new();
      ::BIGNUM *p = ::BN_new();
      ::BIGNUM *q = ::BN_new();
      ::BIGNUM *dmp1 = ::BN_new();
      ::BIGNUM *dmq1 = ::BN_new();
      ::BIGNUM *iqmp = ::BN_new();

      ELLE_ASSERT_NEQ(n, nullptr);
      ELLE_ASSERT_NEQ(e, nullptr);
      ELLE_ASSERT_NEQ(d, nullptr);
      ELLE_ASSERT_NEQ(p, nullptr);
      ELLE_ASSERT_NEQ(q, nullptr);
      ELLE_ASSERT_NEQ(dmp1, nullptr);
      ELLE_ASSERT_NEQ(dmq1, nullptr);
      ELLE_ASSERT_NEQ(iqmp, nullptr);

      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(n);
      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(e);
      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(d);
      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(p);
      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(q);
      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(dmp1);
      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(dmq1);
      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(iqmp);

      archive >> *n
              >> *e
              >> *d
              >> *p
              >> *q
              >> *dmp1
              >> *dmq1
              >> *iqmp;

      // Build the RSA object.
      ::RSA* rsa = ::RSA_new();

      ELLE_ASSERT_NEQ(rsa, nullptr);

      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);

      rsa->n = n;
      rsa->e = e;
      rsa->d = d;
      rsa->p = p;
      rsa->q = q;
      rsa->dmp1 = dmp1;
      rsa->dmq1 = dmq1;
      rsa->iqmp = iqmp;

      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(n);
      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(e);
      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(d);
      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(p);
      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(q);
      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(dmp1);
      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(dmq1);
      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(iqmp);

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
      if (::EVP_PKEY_CTX_set_signature_md(value._context.sign.get(),
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

      ::RSA* rsa =
          infinit::cryptography::rsa::der::decode_private(buffer);

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
  struct hash<infinit::cryptography::rsa::PrivateKey>
  {
    size_t
    operator ()(infinit::cryptography::rsa::PrivateKey const& value) const
    {
      std::stringstream stream;
      {
        elle::serialization::binary::SerializerOut output(stream);
        output.serialize("value", value);
      }

      size_t result = std::hash<std::string>()(stream.str());

      return (result);
    }
  };
}

#endif
