#include <cryptography/rsa/PrivateKey.hh>

#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

#include <elle/Lazy.hh>
#include <elle/log.hh>

#include <cryptography/Error.hh>
#include <cryptography/bn.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/envelope.hh>
#include <cryptography/finally.hh>
#include <cryptography/hash.hh>
#include <cryptography/raw.hh>
#include <cryptography/context.hh>
#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/rsa/Padding.hh>
#include <cryptography/rsa/Seed.hh>
#include <cryptography/rsa/der.hh>
#include <cryptography/rsa/low.hh>
#include <cryptography/rsa/serialization.hh>

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
# include <dopenssl/rsa.hh>
#endif

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
# include <cryptography/_legacy/raw.hh>
#endif

ELLE_LOG_COMPONENT("infinit.cryptography.rsa.PrivateKey");

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace privatekey
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
            return (rsa::der::encode_private(rsa));
          }

          static
          ::RSA*
          decode(elle::ConstWeakBuffer const& buffer)
          {
            return (rsa::der::decode_private(buffer));
          }
        };
      }

      /*-------------.
      | Construction |
      `-------------*/

      PrivateKey::PrivateKey(::EVP_PKEY* key
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
                             , Padding const encryption_padding
                             , Padding const signature_padding
                             , Oneway const oneway
                             , Cipher const envelope_cipher
                             , Mode const envelope_mode
#endif
                            ):
        _key(key)
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        , _encryption_padding(encryption_padding)
        , _signature_padding(signature_padding)
        , _oneway(oneway)
        , _envelope_cipher(envelope_cipher)
        , _envelope_mode(envelope_mode)
#endif
      {
        ELLE_ASSERT_NEQ(key, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.rsa->e, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.rsa->d, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.rsa->p, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.rsa->q, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.rsa->dmp1, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.rsa->dmq1, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.rsa->iqmp, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        if (::EVP_PKEY_type(this->_key->type) != EVP_PKEY_RSA)
          throw Error(
            elle::sprintf("the EVP_PKEY key is not of type RSA: %s",
                          ::EVP_PKEY_type(this->_key->type)));

        this->_check();

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        // Set the current object's version to 0 as it was in the legacy
        // version with the parent class PrivateKey. In addition, keep the
        // subclass (rsa::PrivateKey) format as the latest possible value
        // i.e StaticFormat<PrivateKey>.
        auto _this_dynamic_format =
          static_cast<
            elle::serialize::DynamicFormat<
              infinit::cryptography::rsa::PrivateKey>*>(this);
        _this_dynamic_format->version(0);

        this->_legacy_format =
          elle::serialize::StaticFormat<PrivateKey>::version;
#endif
      }

      PrivateKey::PrivateKey(::RSA* rsa
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
                             , Padding const encryption_padding
                             , Padding const signature_padding
                             , Oneway const oneway
                             , Cipher const envelope_cipher
                             , Mode const envelope_mode):
        _encryption_padding(encryption_padding),
        _signature_padding(signature_padding),
        _oneway(oneway),
        _envelope_cipher(envelope_cipher),
        _envelope_mode(envelope_mode)
#else
                            )
#endif
      {
        ELLE_ASSERT_NEQ(rsa, nullptr);
        ELLE_ASSERT_NEQ(rsa->n, nullptr);
        ELLE_ASSERT_NEQ(rsa->e, nullptr);
        ELLE_ASSERT_NEQ(rsa->d, nullptr);
        ELLE_ASSERT_NEQ(rsa->p, nullptr);
        ELLE_ASSERT_NEQ(rsa->q, nullptr);
        ELLE_ASSERT_NEQ(rsa->dmp1, nullptr);
        ELLE_ASSERT_NEQ(rsa->dmq1, nullptr);
        ELLE_ASSERT_NEQ(rsa->iqmp, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Construct the private key based on the given RSA structure.
        this->_construct(rsa);

        this->_check();

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        auto _this_dynamic_format =
          static_cast<
            elle::serialize::DynamicFormat<
              infinit::cryptography::rsa::PrivateKey>*>(this);
        _this_dynamic_format->version(0);

        this->_legacy_format =
          elle::serialize::StaticFormat<PrivateKey>::version;
#endif
      }

      PrivateKey::PrivateKey(PrivateKey const& other)
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        : elle::serialize::DynamicFormat<PrivateKey>(other)
        , _encryption_padding(other._encryption_padding)
        , _signature_padding(other._signature_padding)
        , _oneway(other._oneway)
        , _envelope_cipher(other._envelope_cipher)
        , _envelope_mode(other._envelope_mode)
        , _legacy_format(other._legacy_format)
#endif
      {
        ELLE_ASSERT_NEQ(other._key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(other._key->pkey.rsa->e, nullptr);
        ELLE_ASSERT_NEQ(other._key->pkey.rsa->d, nullptr);
        ELLE_ASSERT_NEQ(other._key->pkey.rsa->p, nullptr);
        ELLE_ASSERT_NEQ(other._key->pkey.rsa->q, nullptr);
        ELLE_ASSERT_NEQ(other._key->pkey.rsa->dmp1, nullptr);
        ELLE_ASSERT_NEQ(other._key->pkey.rsa->dmq1, nullptr);
        ELLE_ASSERT_NEQ(other._key->pkey.rsa->iqmp, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Duplicate the RSA structure.
        RSA* _rsa = low::RSA_dup(other._key->pkey.rsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(_rsa);

        this->_construct(_rsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(_rsa);

        this->_check();
      }

      PrivateKey::PrivateKey(PrivateKey&& other):
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        elle::serialize::DynamicFormat<PrivateKey>(other),
#endif
        _key(std::move(other._key))
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        , _encryption_padding(std::move(other._encryption_padding))
        , _signature_padding(std::move(other._signature_padding))
        , _oneway(std::move(other._oneway))
        , _envelope_cipher(std::move(other._envelope_cipher))
        , _envelope_mode(std::move(other._envelope_mode))
        , _legacy_format(other._legacy_format)
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
      PrivateKey::_construct(::RSA* rsa)
      {
        ELLE_DEBUG_FUNCTION(rsa);

        ELLE_ASSERT_NEQ(rsa, nullptr);

        // Initialise the private key structure.
        ELLE_ASSERT_EQ(this->_key, nullptr);
        this->_key.reset(::EVP_PKEY_new());

        if (this->_key == nullptr)
          throw Error(
            elle::sprintf("unable to allocate the EVP_PKEY structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Set the rsa structure into the private key.
        if (::EVP_PKEY_assign_RSA(this->_key.get(), rsa) <= 0)
          throw Error(
            elle::sprintf("unable to assign the RSA key to the EVP_PKEY "
                          "structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
      }

      void
      PrivateKey::_check() const
      {
        ELLE_DEBUG_FUNCTION("");

        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->e, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->d, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->p, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->q, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->dmp1, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->dmq1, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->iqmp, nullptr);
      }

# if !defined(INFINIT_CRYPTOGRAPHY_LEGACY)
      elle::Buffer
      PrivateKey::open(elle::ConstWeakBuffer const& code,
                       Cipher const cipher,
                       Mode const mode) const
      {
        ELLE_TRACE_METHOD(cipher, mode);
        ELLE_DUMP("code: %x", code);

        elle::IOStream _code(code.istreambuf());
        std::stringstream _plain;

        this->open(_code, _plain,
                   cipher, mode);

        elle::Buffer plain(_plain.str().data(), _plain.str().length());

        return (plain);
      }

      void
      PrivateKey::open(std::istream& code,
                       std::ostream& plain,
                       Cipher const cipher,
                       Mode const mode) const
      {
        ELLE_TRACE_METHOD(cipher, mode);

        envelope::open(this->_key.get(),
                       cipher::resolve(cipher, mode),
                       code,
                       plain);
      }

      elle::Buffer
      PrivateKey::decrypt(elle::ConstWeakBuffer const& code,
                          Padding const padding) const
      {
        ELLE_TRACE_METHOD(padding);
        ELLE_DUMP("code: %x", code);

        auto prolog =
          [this, padding](::EVP_PKEY_CTX* context)
          {
            padding::pad(context, padding);
          };

        return (raw::asymmetric::decrypt(this->_key.get(),
                                         code,
                                         prolog));
      }

      elle::Buffer
      PrivateKey::sign(elle::ConstWeakBuffer const& plain,
                       Padding const padding,
                       Oneway const oneway) const
      {
        ELLE_TRACE_METHOD(padding, oneway);
        ELLE_DUMP("plain: %x", plain);

        elle::IOStream _plain(plain.istreambuf());

        return (this->sign(_plain,
                           padding, oneway));
      }
# endif

      elle::Buffer
      PrivateKey::sign(std::istream& plain,
                       Padding const padding,
                       Oneway const oneway) const
      {
        ELLE_TRACE_METHOD(padding, oneway);

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        types::EVP_PKEY_CTX context(
          context::create(this->_key.get(), ::EVP_PKEY_sign_init));
        padding::pad(context.get(), padding);

        elle::Buffer digest = hash(plain, oneway);
        return (raw::asymmetric::sign(digest,
                                      context.get(),
                                      ::EVP_PKEY_sign));
#else
        auto prolog =
          [this, padding](::EVP_MD_CTX* context,
                          ::EVP_PKEY_CTX* ctx)
          {
            padding::pad(ctx, padding);
          };

        return (raw::asymmetric::sign(
                  this->_key.get(),
                  oneway::resolve(oneway),
                  plain,
                  prolog));
#endif
      }

      uint32_t
      PrivateKey::size() const
      {
        return (static_cast<uint32_t>(
                  ::EVP_PKEY_size(this->_key.get())));
      }

      uint32_t
      PrivateKey::length() const
      {
        return (static_cast<uint32_t>(
                  ::EVP_PKEY_bits(this->_key.get())));
      }

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
      /*---------.
      | Rotation |
      `---------*/

      PrivateKey::PrivateKey(Seed const& seed)
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

        // Construct the private key based on the given RSA structure.
        this->_construct(rsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);

        this->_check();
      }

      Seed
      PrivateKey::rotate(Seed const& seed) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("seed: %x", seed);

        auto prolog =
          [this](::EVP_PKEY_CTX* ctx)
          {
            padding::pad(ctx, rsa::Padding::none);
          };

        // Note that in these cases, using no RSA padding is not dangerous
        // because (1) the content being rotated is always random (2) the
        // content is always the size of the RSA key's modulus.
        elle::Buffer buffer = raw::asymmetric::rotate(this->_key.get(),
                                                      seed.buffer(),
                                                      prolog);

        return (Seed(buffer, seed.length()));
      }
#endif

      /*----------.
      | Operators |
      `----------*/

      bool
      PrivateKey::operator ==(PrivateKey const& other) const
      {
        if (this == &other)
          return (true);

        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(other._key, nullptr);

        // Compare the public components because it is sufficient to
        // uniquely distinguish keys.
        return (::EVP_PKEY_cmp(this->_key.get(), other._key.get()) == 1);
      }

      PrivateKey&
      PrivateKey::operator= (PrivateKey&& other)
      {
        this->_key = std::move(other._key);
        cryptography::require();
        this->_check();
        return *this;
      }

      /*--------------.
      | Serialization |
      `--------------*/

      PrivateKey::PrivateKey(elle::serialization::SerializerIn& serializer)
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
      PrivateKey::serialize(elle::serialization::Serializer& serializer)
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);

        cryptography::serialize<privatekey::Serialization>(
          serializer,
          this->_key->pkey.rsa);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa, nullptr);
      }

      /*----------.
      | Printable |
      `----------*/

      void
      PrivateKey::print(std::ostream& stream) const
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->e, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->d, nullptr);
        elle::fprintf(
          stream, "PrivateKey(%f)",
          elle::lazy([this] { return privatekey::der::encode(*this); }));
      }
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
      namespace privatekey
      {
        namespace der
        {
          /*----------.
          | Functions |
          `----------*/

          elle::Buffer
          encode(PrivateKey const& K)
          {
            return (rsa::der::encode_private(K.key()->pkey.rsa));
          }

          PrivateKey
          decode(elle::ConstWeakBuffer const& buffer)
          {
            ::RSA* rsa = rsa::der::decode_private(buffer);

            return (PrivateKey(rsa));
          }
        }
      }
    }
  }
}
