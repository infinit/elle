#ifndef INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HXX
# define INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HXX

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)

#  include <cryptography/_legacy/Plain.hh>
#  include <cryptography/_legacy/envelope.hh>
#  include <cryptography/_legacy/serialization.hh>
#  include <cryptography/context.hh>
#  include <cryptography/hash.hh>

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

        types::EVP_PKEY_CTX context(
          context::create(this->_key.get(), ::EVP_PKEY_encrypt_init));
        padding::pad(context.get(), this->_encryption_padding);

        return (Code(envelope::seal(_value,
                                    context.get(),
                                    ::EVP_PKEY_encrypt,
                                    cipher::resolve(this->_envelope_cipher,
                                                    this->_envelope_mode),
                                    oneway::resolve(this->_oneway),
                                    padding::footprint(context.get()))));
      }

      template <typename T>
      bool
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
        elle::IOStream _plain(_value.istreambuf());
        return (this->verify(signature.buffer(),
                             static_cast<std::istream&>(_plain),
                             this->_signature_padding,
                             this->_oneway));
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
#  include <cryptography/Error.hh>
#  include <cryptography/Cipher.hh>
#  include <cryptography/rsa/der.hh>
#  include <cryptography/rsa/Padding.hh>
#  include <cryptography/_legacy/rsa.hh>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::rsa::PublicKey);

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::rsa::PublicKey,
                          archive,
                          value,
                          format)
{
  // Since the original class was the parent PublicKey class that was
  // always in format 0, we can enforce the format's value.
  enforce(format == 0);

  archive << infinit::cryptography::Cryptosystem::rsa;

  // Create the subclass (i.e originally rsa::PublicKey) and set the
  // format to the right value.
  infinit::cryptography::legacy::rsa::Dummy implementation;
  auto& _implementation_dynamic_format =
    static_cast<
      elle::serialize::DynamicFormat<
        infinit::cryptography::legacy::rsa::Dummy>&>(implementation);
  _implementation_dynamic_format.version(value._legacy_format);
  archive << implementation;

  switch (value._legacy_format)
  {
    case 0:
    case 1:
    {
      archive << *value._key->pkey.rsa->n
              << *value._key->pkey.rsa->e;

      break;
    }
    default:
      throw infinit::cryptography::Error(
        elle::sprintf("unknown format '%s'", value._legacy_format));
  }
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::rsa::PublicKey,
                          archive,
                          value,
                          format)
{
  // Emulate deserializing the parent class.
  //
  // Note that the original hierarchy was PublicKey embedding the
  // rsa::PublicKey (this class in practice). Therefore, at the very beginning
  // of this deserialization, we are theoretically in the parent PublicKey
  // class. By deserializing the Dummy class, we intend to deserialize the
  // parent class (that no longer exists: PublicKey). Unfortunately, we are
  // already in the deserialization mechanism, hence the header of the archive
  // has already been fetched, which is the header of the parent PublicKey
  // class. As such, the format provided in this function is not the one for
  // rsa::PublicKey but for the parent PublicKey.
  //
  // The following therefore deserializes Dummy to emulate the hierarchy of
  // classes, takes its format and stores it in the current object for future
  // use, in particular in the Save() serialization method, to know how the
  // object must be saved.
  enforce(format == 0);

  // Extract the cryptosystem which was in the parent class PublicKey.
  infinit::cryptography::Cryptosystem cryptosystem;
  archive >> cryptosystem;
  ELLE_ASSERT_EQ(cryptosystem, infinit::cryptography::Cryptosystem::rsa);

  // Extract the subclass: rsa::PublicKey.
  infinit::cryptography::legacy::rsa::Dummy implementation;
  archive >> implementation;

  // Report the subclass' format in the current one since the modern
  // rsa::PublicKey class plays the role of both classes.
  auto& _implementation_dynamic_format =
    static_cast<
      elle::serialize::DynamicFormat<
        infinit::cryptography::legacy::rsa::Dummy>&>(implementation);
  value._legacy_format = _implementation_dynamic_format.version();

  switch (value._legacy_format)
  {
    case 0:
    case 1:
    {
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
      switch (value._legacy_format)
      {
        case 0:
        {
          value._encryption_padding =
            infinit::cryptography::rsa::Padding::oaep;
          value._signature_padding =
            infinit::cryptography::rsa::Padding::pkcs1;
          value._oneway =
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
          value._oneway =
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
      value._key = infinit::cryptography::rsa::_details::build_evp(rsa);

      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);

      value._check();

      break;
    }
    default:
      throw infinit::cryptography::Error(
        elle::sprintf("unknown format '%s'", value._legacy_format));
  }
}

# else

#  include <elle/Lazy.hh>
#  include <elle/serialization/binary.hh>
#  include <elle/serialization/json.hh>
#  include <elle/utility/Move.hh>

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      template <>
      inline
      bool
      PublicKey::verify<elle::Buffer>(elle::ConstWeakBuffer const& signature,
                                      elle::Buffer const& plain) const
      {
        return this->verify(signature, elle::ConstWeakBuffer(plain));
      }

      template <typename T>
      bool
      PublicKey::verify(elle::ConstWeakBuffer const& signature,
                        T const& o) const
      {
        auto data = this->_verify_data(signature, o);
        return this->_verify(data.first, data.second);
      }

      template <typename T>
      std::function <bool ()>
      PublicKey::verify_async(elle::ConstWeakBuffer const& signature,
                              T const& o) const
      {
        return [self = this->shared_from_this(),
                data = this->_verify_data(signature, o)]
        {
          return self->_verify(data.first, data.second);
        };
      }

      template <typename T>
      std::pair<elle::Buffer, elle::Buffer>
      PublicKey::_verify_data(elle::ConstWeakBuffer const& signature,
                              T const& o) const
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.rsa.PublicKey");
        ELLE_TRACE_SCOPE("%s: verify %s", this, o);
        elle::IOStream input(signature.istreambuf());
        auto version =
          elle::serialization::binary::deserialize<elle::Version>(input, false);
        auto s = elle::Buffer(std::string{std::istreambuf_iterator<char>(input),
                                          std::istreambuf_iterator<char>()});
        auto serialized =
          elle::serialization::binary::serialize(o, version, false);
        ELLE_DUMP(
          "data: %s",
          elle::lazy([&] {
              std::stringstream out;
              elle::serialization::json::serialize(o, out, version,
                                                   false, true);
              return out.str();
            }));
        ELLE_DUMP("serialization version: %s", version);
        ELLE_DUMP("serialized data: %s", serialized);
        ELLE_DUMP("signature: %s", s);
        return std::make_pair(std::move(s), std::move(serialized));
      }
    }
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
        elle::serialization::binary::SerializerOut output(stream);
        output.serialize("value", value);
      }

      size_t result = std::hash<std::string>()(stream.str());

      return (result);
    }
  };
}

#endif
