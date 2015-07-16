#include <cryptography/rsa/PrivateKey.hh>

#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

#include <elle/log.hh>

#include <cryptography/Error.hh>
#include <cryptography/bn.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/envelope.hh>
#include <cryptography/finally.hh>
#include <cryptography/hash.hh>
#include <cryptography/raw.hh>
#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/rsa/Padding.hh>
#include <cryptography/rsa/Seed.hh>
#include <cryptography/rsa/context.hh>
#include <cryptography/rsa/der.hh>
#include <cryptography/rsa/low.hh>
#include <cryptography/rsa/serialization.hh>

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
# include <dopenssl/rsa.hh>
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
            return der::encode_private(rsa);
          }

          static
          ::RSA*
          decode(elle::ConstWeakBuffer const& buffer)
          {
            return der::decode_private(buffer);
          }
        };
      }

      /*-------------.
      | Construction |
      `-------------*/

      PrivateKey::PrivateKey(::EVP_PKEY* key,
                             Padding const encryption_padding,
                             Padding const signature_padding,
                             Oneway const digest_algorithm):
        _key(key),
        _encryption_padding(encryption_padding),
        _signature_padding(signature_padding),
        _digest_algorithm(digest_algorithm)
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

        // Prepare the cryptographic contexts.
        this->_prepare();

        this->_check();
      }

      PrivateKey::PrivateKey(::RSA* rsa,
                             Padding const encryption_padding,
                             Padding const signature_padding,
                             Oneway const digest_algorithm):
        _encryption_padding(encryption_padding),
        _signature_padding(signature_padding),
        _digest_algorithm(digest_algorithm)
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

        // Prepare the cryptographic contexts.
        this->_prepare();

        this->_check();
      }

      PrivateKey::PrivateKey(PrivateKey const& other):
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        elle::serialize::DynamicFormat<PrivateKey>(other),
#endif
        _encryption_padding(other._encryption_padding),
        _signature_padding(other._signature_padding),
        _digest_algorithm(other._digest_algorithm)
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

        // Prepare the cryptographic contexts.
        this->_prepare();

        this->_check();
      }

      PrivateKey::PrivateKey(PrivateKey&& other):
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        elle::serialize::DynamicFormat<PrivateKey>(other),
#endif
        _key(std::move(other._key)),
        _encryption_padding(std::move(other._encryption_padding)),
        _signature_padding(std::move(other._signature_padding)),
        _digest_algorithm(std::move(other._digest_algorithm))
      {
        this->_context.decrypt = std::move(other._context.decrypt);
        this->_context.sign = std::move(other._context.sign);
#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        this->_context.rotate = std::move(other._context.rotate);
#endif

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
      PrivateKey::_prepare()
      {
        ELLE_DEBUG_FUNCTION("");

        ELLE_ASSERT_NEQ(this->_key, nullptr);

        // Prepare the decrypt context.
        ELLE_ASSERT_EQ(this->_context.decrypt, nullptr);
        this->_context.decrypt.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_decrypt_init,
                          this->_encryption_padding));

        // Prepare the sign context.
        ELLE_ASSERT_EQ(this->_context.sign, nullptr);
        this->_context.sign.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_sign_init,
                          this->_signature_padding));

        if (this->_signature_padding == Padding::pss)
        {
          if (::EVP_PKEY_CTX_ctrl(this->_context.sign.get(),
                                  EVP_PKEY_RSA,
                                  -1,
                                  EVP_PKEY_CTRL_RSA_PSS_SALTLEN,
                                  -2,
                                  nullptr) <= 0)
            throw Error(
              elle::sprintf("unable to set the EVP_PKEY context's PSS "
                            "salt length: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));
        }

        if (::EVP_PKEY_CTX_set_signature_md(
              this->_context.sign.get(),
              (void*)oneway::resolve(this->_digest_algorithm)) <= 0)
          throw Error(
            elle::sprintf("unable to set the EVP_PKEY context's digest "
                          "function: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        // Note that in these cases, using no RSA padding is not dangerous
        // because (1) the content being rotated is always random i.e cannot be
        // guessed because produced by a human being (2) the content is always
        // the size of the RSA key's modulus.

        // Prepare the rotate context.
        ELLE_ASSERT_EQ(this->_context.rotate, nullptr);
        this->_context.rotate.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_sign_init,
                          Padding::none));
#endif
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
      PrivateKey::open(elle::ConstWeakBuffer const& code) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("code: %x", code);

        return (envelope::open(code,
                               this->_context.decrypt.get(),
                               ::EVP_PKEY_decrypt));
      }

      elle::Buffer
      PrivateKey::decrypt(elle::ConstWeakBuffer const& code) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("code: %x", code);

        return (raw::asymmetric::decrypt(code,
                                         this->_context.decrypt.get(),
                                         ::EVP_PKEY_decrypt));
      }

      elle::Buffer
      PrivateKey::sign(elle::ConstWeakBuffer const& plain) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("plain: %x", plain);

        elle::IOStream _plain(plain.istreambuf());

        return (this->sign(_plain));
      }
# endif

      elle::Buffer
      PrivateKey::sign(std::istream& plain) const
      {
        ELLE_TRACE_METHOD("");

        elle::Buffer digest = hash(plain, this->_digest_algorithm);

        return (raw::asymmetric::sign(digest,
                                      this->_context.sign.get(),
                                      ::EVP_PKEY_sign));
      }

      elle::Natural32
      PrivateKey::size() const
      {
        return (static_cast<elle::Natural32>(
                  ::EVP_PKEY_size(this->_key.get())));
      }

      elle::Natural32
      PrivateKey::length() const
      {
        return (static_cast<elle::Natural32>(
                  ::EVP_PKEY_bits(this->_key.get())));
      }

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
      /*---------.
      | Rotation |
      `---------*/

      PrivateKey::PrivateKey(Seed const& seed,
                             Padding const encryption_padding,
                             Padding const signature_padding,
                             Oneway const digest_algorithm):
        _encryption_padding(encryption_padding),
        _signature_padding(signature_padding),
        _digest_algorithm(digest_algorithm)
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

        // Prepare the cryptographic contexts.
        this->_prepare();

        this->_check();
      }

      Seed
      PrivateKey::rotate(Seed const& seed) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("seed: %x", seed);

        return (Seed(raw::asymmetric::rotate(seed.buffer(),
                                             this->_context.rotate.get(),
                                             ::EVP_PKEY_sign),
                     seed.length()));
      }
#endif

      /*----------.
      | Operators |
      `----------*/

      elle::Boolean
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

        this->_prepare();
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

        serializer.serialize("encryption padding", this->_encryption_padding);
        serializer.serialize("signature padding", this->_signature_padding);
        serializer.serialize("digest algorithm", this->_digest_algorithm);
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

        stream << "("
               << *this->_key->pkey.rsa->n
               << ", "
               << *this->_key->pkey.rsa->e
               << ", "
               << *this->_key->pkey.rsa->d
               << ")";

        stream << "["
               << this->_encryption_padding
               << ", "
               << this->_signature_padding
               << ", "
               << this->_digest_algorithm
               << "]";
      }
    }
  }
}
