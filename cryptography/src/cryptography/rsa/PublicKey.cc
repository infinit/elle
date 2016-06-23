#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include <functional>

#include <elle/Error.hh>
#include <elle/Lazy.hh>
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
      namespace _details
      {
        void
        raise(std::string const& message)
        {
          throw Error(
            elle::sprintf(
              "%s: %s",
              message, ::ERR_error_string(ERR_get_error(), nullptr)));
        }

        types::EVP_PKEY
        build_evp(::RSA* rsa)
        {
          cryptography::require();
          ELLE_ASSERT(rsa);
          types::EVP_PKEY key(EVP_PKEY_new());
          if (!key)
            raise("unable to allocate the EVP_PKEY structure");
          if (::EVP_PKEY_assign_RSA(key.get(), rsa) <= 0)
            raise("unable to assign the RSA key to the EVP_PKEY structure");
          return key;
        }
      }

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
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        : PublicKey(
          _details::build_evp(
            low::RSA_priv2pub(k.key().get()->pkey.rsa)).release(),
          k.encryption_padding(),
          k.signature_padding(),
          k.oneway(),
          k.envelope_cipher(),
          k.envelope_mode())
#else
        : PublicKey(
          _details::build_evp(
            low::RSA_priv2pub(k.key().get()->pkey.rsa)).release())
#endif
      {}

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
      PublicKey::PublicKey(::EVP_PKEY* key,
                           Padding const encryption_padding,
                           Padding const signature_padding,
                           Oneway const oneway,
                           Cipher const envelope_cipher,
                           Mode const envelope_mode,
                           uint16_t legacy_format,
                           uint16_t dynamic_format)
        : _key(key)
        , _encryption_padding(encryption_padding)
        , _signature_padding(signature_padding)
        , _oneway(oneway)
        , _envelope_cipher(envelope_cipher)
        , _envelope_mode(envelope_mode)
        , _legacy_format(legacy_format)
#else
      PublicKey::PublicKey(::EVP_PKEY* key)
        : _key(key)
#endif
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();
        if (::EVP_PKEY_type(this->_key->type) != EVP_PKEY_RSA)
          throw Error(
            elle::sprintf("the EVP_PKEY key is not of type RSA: %s",
                          ::EVP_PKEY_type(this->_key->type)));
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        auto _this_dynamic_format =
          static_cast<
            elle::serialize::DynamicFormat<
              infinit::cryptography::rsa::PublicKey>*>(this);
        _this_dynamic_format->version(dynamic_format);
#endif
        this->_check();
      }

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
      PublicKey::PublicKey(::RSA* rsa,
                           Padding const encryption_padding,
                           Padding const signature_padding,
                           Oneway const oneway,
                           Cipher const envelope_cipher,
                           Mode const envelope_mode)
        : PublicKey(_details::build_evp(rsa).release(),
                    encryption_padding,
                    signature_padding,
                    oneway,
                    envelope_cipher,
                    envelope_mode)
#else
      PublicKey::PublicKey(::RSA* rsa)
        : PublicKey(_details::build_evp(rsa).release())
#endif
      {}

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
      PublicKey::PublicKey(PublicKey const& other)
        : PublicKey(_details::build_evp(
                      low::RSA_dup(other._key->pkey.rsa)).release(),
                    other._encryption_padding,
                    other._signature_padding,
                    other._oneway,
                    other._envelope_cipher,
                    other._envelope_mode,
                    other._legacy_format,
                    static_cast< elle::serialize::DynamicFormat<
                      infinit::cryptography::rsa::PublicKey> const&>
                      (other).version())
#else
      PublicKey::PublicKey(PublicKey const& other)
        : PublicKey(low::RSA_dup(other._key->pkey.rsa))
#endif
      {}

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
      PublicKey::PublicKey(PublicKey&& other)
        : PublicKey(other._key.release(),
                    other._encryption_padding,
                    other._signature_padding,
                    other._oneway,
                    other._envelope_cipher,
                    other._envelope_mode,
                    other._legacy_format,
                    static_cast< elle::serialize::DynamicFormat<
                      infinit::cryptography::rsa::PublicKey>&>(other).version())
#else
      PublicKey::PublicKey(PublicKey&& other)
        : PublicKey(other._key.release())
#endif
      {}

      /*--------.
      | Methods |
      `--------*/

      void
      PublicKey::_check() const
      {
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
      PublicKey::seal(elle::ConstWeakBuffer const& plain,
                      Cipher const cipher,
                      Mode const mode) const
      {
        ELLE_DUMP("plain: %x", plain);

        elle::IOStream _plain(plain.istreambuf());
        std::stringstream _code;

        this->seal(_plain, _code,
                   cipher, mode);

        elle::Buffer code(_code.str().data(), _code.str().length());

        return (code);
      }

      void
      PublicKey::seal(std::istream& plain,
                      std::ostream& code,
                      Cipher const cipher,
                      Mode const mode) const
      {
        envelope::seal(this->_key.get(),
                       cipher::resolve(cipher, mode),
                       plain,
                       code);
      }

      elle::Buffer
      PublicKey::encrypt(elle::ConstWeakBuffer const& plain,
                         Padding const padding) const
      {
        ELLE_DUMP("plain: %x", plain);

        auto prolog =
          [this, padding](::EVP_PKEY_CTX* context)
          {
            padding::pad(context, padding);
          };

        return (raw::asymmetric::encrypt(this->_key.get(),
                                         plain,
                                         prolog));
      }

      bool
      PublicKey::verify(elle::ConstWeakBuffer const& signature,
                        elle::ConstWeakBuffer const& plain,
                        Padding const padding,
                        Oneway const oneway) const
      {
        ELLE_DUMP("signature: %x", signature);
        ELLE_DUMP("plain: %x", plain);

        elle::IOStream _plain(plain.istreambuf());

        return (this->verify(signature, _plain,
                             padding, oneway));
      }
#endif

      bool
      PublicKey::verify(elle::ConstWeakBuffer const& signature,
                        std::istream& plain,
                        Padding const padding,
                        Oneway const oneway) const
      {
        ELLE_DUMP("signature: %x", signature);

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        types::EVP_PKEY_CTX context(
          context::create(this->_key.get(), ::EVP_PKEY_verify_init));
        padding::pad(context.get(), padding);

        elle::Buffer digest = hash(plain, oneway);
        return (raw::asymmetric::verify(signature,
                                        digest,
                                        context.get(),
                                        ::EVP_PKEY_verify));
#else
        auto prolog =
          [this, padding](::EVP_MD_CTX* context,
                          ::EVP_PKEY_CTX* ctx)
          {
            padding::pad(ctx, padding);
          };

        return (raw::asymmetric::verify(
                  this->_key.get(),
                  oneway::resolve(oneway),
                  signature,
                  plain,
                  prolog));
#endif
      }

      uint32_t
      PublicKey::size() const
      {
        return (static_cast<uint32_t>(
                  ::EVP_PKEY_size(this->_key.get())));
      }

      uint32_t
      PublicKey::length() const
      {
        return (static_cast<uint32_t>(
                  ::EVP_PKEY_bits(this->_key.get())));
      }

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
      /*---------.
      | Rotation |
      `---------*/

      RSA*
      rsa_from_seed(Seed const& seed)
      {
        // Deduce the RSA key from the given seed
        ::RSA* rsa = nullptr;
        if ((rsa = ::dRSA_deduce_privatekey(
               seed.length(),
               static_cast<unsigned char const*>(seed.buffer().contents()),
               seed.buffer().size())) == nullptr)
          _details::raise("unable to deduce the RSA key from the given seed");
        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);
        // Extract the public key
        RSA* pub = low::RSA_priv2pub(rsa);
        ::RSA_free(rsa);
        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);
        return pub;
      }

      PublicKey::PublicKey(Seed const& seed)
        : PublicKey(_details::build_evp(rsa_from_seed(seed)).release())
      {}

      Seed
      PublicKey::unrotate(Seed const& seed) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("seed: %x", seed);

        auto prolog =
          [this](::EVP_PKEY_CTX* ctx)
          {
            padding::pad(ctx, rsa::Padding::none);
          };

        // The unrotate operation does not rely on padding. Not that relying on
        // textbook RSA is considered foolish. In this case however, restricting
        // the rotation/derivation to content of the size of the RSA key's
        // modulus makes it secure.
        elle::Buffer buffer = raw::asymmetric::unrotate(this->_key.get(),
                                                        seed.buffer(),
                                                        prolog);

        return (Seed(buffer, seed.length()));
      }
#endif

      /*----------.
      | Operators |
      `----------*/

      bool
      PublicKey::operator ==(PublicKey const& other) const
      {
        if (this == &other)
          return (true);
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(other._key, nullptr);
        return (::EVP_PKEY_cmp(this->_key.get(), other._key.get()) == 1);
      }

      bool
      PublicKey::operator <(PublicKey const& other) const
      {
        return publickey::der::encode(*this) < publickey::der::encode(other);
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
        elle::fprintf(
          stream, "PublicKey(%f)",
          elle::lazy([this] { return publickey::der::encode(*this); }));
      }

      /*-------.
      | Legacy |
      `-------*/

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
      bool
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
          decode(elle::ConstWeakBuffer const& buffer)
          {
            ::RSA* rsa = rsa::der::decode_public(buffer);

            return (PublicKey(rsa));
          }
        }
      }
    }
  }
}
