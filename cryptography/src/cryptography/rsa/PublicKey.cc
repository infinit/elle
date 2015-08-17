#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include <functional>

#include <elle/Error.hh>
#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/printf.hh>

#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/rsa/Seed.hh>
#include <cryptography/rsa/Padding.hh>
#include <cryptography/rsa/low.hh>
#include <cryptography/rsa/serialization.hh>
#include <cryptography/rsa/der.hh>
#include <cryptography/Error.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/bn.hh>
#include <cryptography/raw.hh>
#include <cryptography/envelope.hh>
#include <cryptography/context.hh>
#include <cryptography/finally.hh>
#include <cryptography/hash.hh>

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
# include <dopenssl/rsa.hh>
#endif

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
# include <cryptography/_legacy/raw.hh>
#endif

ELLE_LOG_COMPONENT("infinit.cryptography.rsa.PublicKey");

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace publickey
      {
        /*--------------.
        | Serialization |
        `--------------*/

        struct Serialization:
          public rsa::serialization::RSA
        {
          static
          elle::Buffer
          encode(::RSA* rsa)
          {
            return (rsa::der::encode_public(rsa));
          }

          static
          ::RSA*
          decode(elle::ConstWeakBuffer const& buffer)
          {
            return (rsa::der::decode_public(buffer));
          }
        };
      }

      /*-------------.
      | Construction |
      `-------------*/

      PublicKey::PublicKey(PrivateKey const& k)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Set the key parameters.
        this->_encryption_padding = k.encryption_padding();
        this->_signature_padding = k.signature_padding();
        this->_digest_algorithm = k.digest_algorithm();
        this->_envelope_cipher = k.envelope_cipher();
        this->_envelope_mode = k.envelope_mode();

        // Extract the public key only.
        RSA* _rsa = low::RSA_priv2pub(k.key().get()->pkey.rsa);

        ELLE_ASSERT_NEQ(_rsa->n, nullptr);
        ELLE_ASSERT_NEQ(_rsa->e, nullptr);
        ELLE_ASSERT_EQ(_rsa->d, nullptr);
        ELLE_ASSERT_EQ(_rsa->p, nullptr);
        ELLE_ASSERT_EQ(_rsa->q, nullptr);
        ELLE_ASSERT_EQ(_rsa->dmp1, nullptr);
        ELLE_ASSERT_EQ(_rsa->dmq1, nullptr);
        ELLE_ASSERT_EQ(_rsa->iqmp, nullptr);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(_rsa);

        // Construct the public key based on the given RSA structure whose
        // ownership is retained.
        this->_construct(_rsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(_rsa);

        this->_check();

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        // Set the current object's version to 0 as it was in the legacy
        // version with the parent class PublicKey. In addition, keep the
        // subclass (rsa::PublicKey) format as the latest possible value
        // i.e StaticFormat<PublicKey>.
        auto _this_dynamic_format =
          static_cast<
            elle::serialize::DynamicFormat<
              infinit::cryptography::rsa::PublicKey>*>(this);
        _this_dynamic_format->version(0);

        this->_legacy_format =
          elle::serialize::StaticFormat<PublicKey>::version;
#endif
      }

      PublicKey::PublicKey(::EVP_PKEY* key,
                           Padding const encryption_padding,
                           Padding const signature_padding,
                           Oneway const digest_algorithm,
                           Cipher const envelope_cipher,
                           Mode const envelope_mode):
        _key(key),
        _encryption_padding(encryption_padding),
        _signature_padding(signature_padding),
        _digest_algorithm(digest_algorithm),
        _envelope_cipher(envelope_cipher),
        _envelope_mode(envelope_mode)
      {
        ELLE_ASSERT_NEQ(key, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.rsa->e, nullptr);
        ELLE_ASSERT_EQ(key->pkey.rsa->d, nullptr);
        ELLE_ASSERT_EQ(key->pkey.rsa->p, nullptr);
        ELLE_ASSERT_EQ(key->pkey.rsa->q, nullptr);
        ELLE_ASSERT_EQ(key->pkey.rsa->dmp1, nullptr);
        ELLE_ASSERT_EQ(key->pkey.rsa->dmq1, nullptr);
        ELLE_ASSERT_EQ(key->pkey.rsa->iqmp, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        if (::EVP_PKEY_type(this->_key->type) != EVP_PKEY_RSA)
          throw Error(
            elle::sprintf("the EVP_PKEY key is not of type RSA: %s",
                          ::EVP_PKEY_type(this->_key->type)));

        this->_check();

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        auto _this_dynamic_format =
          static_cast<
            elle::serialize::DynamicFormat<
              infinit::cryptography::rsa::PublicKey>*>(this);
        _this_dynamic_format->version(0);

        this->_legacy_format =
          elle::serialize::StaticFormat<PublicKey>::version;
#endif
      }

      PublicKey::PublicKey(::RSA* rsa,
                           Padding const encryption_padding,
                           Padding const signature_padding,
                           Oneway const digest_algorithm,
                           Cipher const envelope_cipher,
                           Mode const envelope_mode):
        _encryption_padding(encryption_padding),
        _signature_padding(signature_padding),
        _digest_algorithm(digest_algorithm),
        _envelope_cipher(envelope_cipher),
        _envelope_mode(envelope_mode)
      {
        ELLE_ASSERT_NEQ(rsa, nullptr);
        ELLE_ASSERT_NEQ(rsa->n, nullptr);
        ELLE_ASSERT_NEQ(rsa->e, nullptr);
        ELLE_ASSERT_EQ(rsa->d, nullptr);
        ELLE_ASSERT_EQ(rsa->p, nullptr);
        ELLE_ASSERT_EQ(rsa->q, nullptr);
        ELLE_ASSERT_EQ(rsa->dmp1, nullptr);
        ELLE_ASSERT_EQ(rsa->dmq1, nullptr);
        ELLE_ASSERT_EQ(rsa->iqmp, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Construct the public key based on the given RSA structure.
        this->_construct(rsa);

        this->_check();

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        auto _this_dynamic_format =
          static_cast<
            elle::serialize::DynamicFormat<
              infinit::cryptography::rsa::PublicKey>*>(this);
        _this_dynamic_format->version(0);

        this->_legacy_format =
          elle::serialize::StaticFormat<PublicKey>::version;
#endif
      }

      PublicKey::PublicKey(PublicKey const& other):
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        elle::serialize::DynamicFormat<PublicKey>(other),
#endif
        _encryption_padding(other._encryption_padding),
        _signature_padding(other._signature_padding),
        _digest_algorithm(other._digest_algorithm),
        _envelope_cipher(other._envelope_cipher),
        _envelope_mode(other._envelope_mode)
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        , _legacy_format(other._legacy_format)
#endif
      {
        ELLE_ASSERT_NEQ(other._key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(other._key->pkey.rsa->e, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Duplicate the RSA structure.
        RSA* _rsa = low::RSA_dup(other._key->pkey.rsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(_rsa);

        this->_construct(_rsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(_rsa);

        this->_check();
      }

      PublicKey::PublicKey(PublicKey&& other):
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        elle::serialize::DynamicFormat<PublicKey>(other),
#endif
        _key(std::move(other._key)),
        _encryption_padding(std::move(other._encryption_padding)),
        _signature_padding(std::move(other._signature_padding)),
        _digest_algorithm(std::move(other._digest_algorithm)),
        _envelope_cipher(std::move(other._envelope_cipher)),
        _envelope_mode(std::move(other._envelope_mode))
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        , _legacy_format(std::move(other._legacy_format))
#endif
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        this->_check();
      }

      /*--------.
      | Methods |
      `--------*/

      void
      PublicKey::_construct(::RSA* rsa)
      {
        ELLE_DEBUG_FUNCTION(rsa);

        ELLE_ASSERT_NEQ(rsa, nullptr);

        // Initialise the public key structure.
        ELLE_ASSERT_EQ(this->_key, nullptr);
        this->_key.reset(::EVP_PKEY_new());

        if (this->_key == nullptr)
          throw Error(
            elle::sprintf("unable to allocate the EVP_PKEY structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Set the rsa structure into the public key.
        if (::EVP_PKEY_assign_RSA(this->_key.get(), rsa) <= 0)
          throw Error(
            elle::sprintf("unable to assign the RSA key to the EVP_PKEY "
                          "structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
      }

      void
      PublicKey::_check() const
      {
        ELLE_DEBUG_FUNCTION("");

        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->e, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->d, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->p, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->q, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->dmp1, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->dmq1, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->iqmp, nullptr);
      }

#if !defined(INFINIT_CRYPTOGRAPHY_LEGACY)
      elle::Buffer
      PublicKey::seal(elle::ConstWeakBuffer const& plain) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("plain: %x", plain);

        elle::IOStream _plain(plain.istreambuf());
        std::stringstream _code;

        this->seal(_plain, _code);

        elle::Buffer code(_code.str().data(), _code.str().length());

        return (code);
      }

      void
      PublicKey::seal(std::istream& plain,
                      std::ostream& code) const
      {
        ELLE_TRACE_METHOD("");

        envelope::seal(this->_key.get(),
                       cipher::resolve(this->_envelope_cipher,
                                       this->_envelope_mode),
                       plain,
                       code);
      }

      elle::Buffer
      PublicKey::encrypt(elle::ConstWeakBuffer const& plain) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("plain: %x", plain);

        auto prolog =
          [this](::EVP_PKEY_CTX* context)
          {
            padding::pad(context, this->_encryption_padding);
          };

        return (raw::asymmetric::encrypt(this->_key.get(),
                                         plain,
                                         prolog));
      }

      elle::Boolean
      PublicKey::verify(elle::ConstWeakBuffer const& signature,
                        elle::ConstWeakBuffer const& plain) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("signature: %x", signature);
        ELLE_DUMP("plain: %x", plain);

        elle::IOStream _plain(plain.istreambuf());

        return (this->verify(signature, _plain));
      }
#endif

      elle::Boolean
      PublicKey::verify(elle::ConstWeakBuffer const& signature,
                        std::istream& plain) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("signature: %x", signature);

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        types::EVP_PKEY_CTX context(
          context::create(this->_key.get(), ::EVP_PKEY_verify_init));
        padding::pad(context.get(), this->_signature_padding);

        elle::Buffer digest = hash(plain, this->_digest_algorithm);
        return (raw::asymmetric::verify(signature,
                                        digest,
                                        context.get(),
                                        ::EVP_PKEY_verify));
#else
        auto prolog =
          [this](::EVP_MD_CTX* context,
                 ::EVP_PKEY_CTX* ctx)
          {
            padding::pad(ctx, this->_signature_padding);
          };

        return (raw::asymmetric::verify(
                  this->_key.get(),
                  oneway::resolve(this->_digest_algorithm),
                  signature,
                  plain,
                  prolog));
#endif
      }

      elle::Natural32
      PublicKey::size() const
      {
        return (static_cast<elle::Natural32>(
                  ::EVP_PKEY_size(this->_key.get())));
      }

      elle::Natural32
      PublicKey::length() const
      {
        return (static_cast<elle::Natural32>(
                  ::EVP_PKEY_bits(this->_key.get())));
      }

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
      /*---------.
      | Rotation |
      `---------*/

      PublicKey::PublicKey(Seed const& seed,
                           Padding const encryption_padding,
                           Padding const signature_padding,
                           Oneway const digest_algorithm,
                           Cipher const envelope_cipher,
                           Mode const envelope_mode):
        _encryption_padding(encryption_padding),
        _signature_padding(signature_padding),
        _digest_algorithm(digest_algorithm),
        _envelope_cipher(envelope_cipher),
        _envelope_mode(envelope_mode)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Deduce the RSA key from the given seed.
        ::RSA* rsa = nullptr;

        if ((rsa = ::dRSA_deduce_privatekey(
               seed.length(),
               static_cast<unsigned char const*>(seed.buffer().contents()),
               seed.buffer().size())) == nullptr)
          throw Error(
            elle::sprintf("unable to deduce the RSA key from the given "
                          "seed: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);

        // Extract the public key only.
        RSA* _rsa = low::RSA_priv2pub(rsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(_rsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);
        ::RSA_free(rsa);

        // Construct the public key based on the given RSA structure whose
        // ownership is retained.
        this->_construct(_rsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(_rsa);

        this->_check();
      }

      Seed
      PublicKey::unrotate(Seed const& seed) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("seed: %x", seed);

        // The unrotate operation does not rely on padding. Not that relying on
        // textbook RSA is considered foolish. In this case however, restricting
        // the rotation/derivation to content of the size of the RSA key's
        // modulus makes it secure.
        elle::Buffer buffer = raw::asymmetric::unrotate(this->_key.get(),
                                                        seed.buffer());

        return (Seed(buffer, seed.length()));
      }
#endif

      /*----------.
      | Operators |
      `----------*/

      elle::Boolean
      PublicKey::operator ==(PublicKey const& other) const
      {
        if (this == &other)
          return (true);

        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(other._key, nullptr);

        return (::EVP_PKEY_cmp(this->_key.get(), other._key.get()) == 1);
      }

      /*--------------.
      | Serialization |
      `--------------*/

      PublicKey::PublicKey(elle::serialization::SerializerIn& serializer)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Allocate the EVP key to receive the deserialized's RSA structure.
        this->_key.reset(::EVP_PKEY_new());

        // Set the EVP key as being of type RSA.
        if (::EVP_PKEY_set_type(this->_key.get(), EVP_PKEY_RSA) <= 0)
          throw Error(
            elle::sprintf("unable to set the EVP key's type: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        this->serialize(serializer);

        this->_check();
      }

      void
      PublicKey::serialize(elle::serialization::Serializer& serializer)
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);

        cryptography::serialize<publickey::Serialization>(
          serializer,
          this->_key->pkey.rsa);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa, nullptr);

        serializer.serialize("encryption padding", this->_encryption_padding);
        serializer.serialize("signature padding", this->_signature_padding);
        serializer.serialize("digest algorithm", this->_digest_algorithm);
        serializer.serialize("envelope cipher", this->_envelope_cipher);
        serializer.serialize("envelope mode", this->_envelope_mode);
      }

      /*----------.
      | Printable |
      `----------*/

      void
      PublicKey::print(std::ostream& stream) const
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->e, nullptr);

        stream << "("
               << *this->_key->pkey.rsa->n
               << ", "
               << *this->_key->pkey.rsa->e
               << ")";

        stream << "["
               << this->_encryption_padding
               << ", "
               << this->_signature_padding
               << ", "
               << this->_digest_algorithm
               << ", "
               << this->_envelope_cipher
               << ", "
               << this->_envelope_mode
               << "]";
      }

      /*-------.
      | Legacy |
      `-------*/

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
      elle::Boolean
      PublicKey::operator <(PublicKey const& other) const
      {
        if (this == &other)
          return (false);
        return std::hash<PublicKey>()(*this) < std::hash<PublicKey>()(other);
      }
#endif
    }
  }
}

//
// ---------- DER -------------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace publickey
      {
        namespace der
        {
          /*----------.
          | Functions |
          `----------*/

          elle::Buffer
          encode(PublicKey const& K)
          {
            return (rsa::der::encode_public(K.key()->pkey.rsa));
          }

          PublicKey
          decode(elle::ConstWeakBuffer const& buffer,
                 Padding const encryption_padding,
                 Padding const signature_padding,
                 Oneway const digest_algorithm,
                 Cipher const envelope_cipher,
                 Mode const envelope_mode)
          {
            ::RSA* rsa = rsa::der::decode_public(buffer);

            return (PublicKey(rsa,
                              encryption_padding,
                              signature_padding,
                              digest_algorithm,
                              envelope_cipher,
                              envelope_mode));
          }
        }
      }
    }
  }
}
