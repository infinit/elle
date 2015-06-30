#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include <elle/Error.hh>
#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/printf.hh>

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
# include <dopenssl/rsa.hh>
#endif

#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/rsa/Seed.hh>
#include <cryptography/rsa/Padding.hh>
#include <cryptography/rsa/context.hh>
#include <cryptography/rsa/low.hh>
#include <cryptography/rsa/serialization.hh>
#include <cryptography/rsa/der.hh>
#include <cryptography/Digest.hh>
#include <cryptography/Code.hh>
#include <cryptography/Exception.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/bn.hh>
#include <cryptography/evp.hh>
#include <cryptography/finally.hh>
#include <cryptography/hash.hh>

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
            return der::encode_public(rsa);
          }

          static
          ::RSA*
          decode(elle::ConstWeakBuffer const& buffer)
          {
            return der::decode_public(buffer);
          }
        };
      }

      /*-------------.
      | Construction |
      `-------------*/

      PublicKey::PublicKey(PrivateKey const& k,
                           Cipher const envelope_cipher,
                           Mode const envelope_mode)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Set the key parameters.
        this->_encryption_padding = k.encryption_padding();
        this->_signature_padding = k.signature_padding();
        this->_digest_algorithm = k.digest_algorithm();
        this->_envelope_cipher = envelope_cipher;
        this->_envelope_mode = envelope_mode;

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

        // Prepare the cryptographic contexts.
        this->_prepare();

        this->_check();
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
          throw Exception(
            elle::sprintf("the EVP_PKEY key is not of type RSA: %s",
                          ::EVP_PKEY_type(this->_key->type)));

        // Prepare the cryptographic contexts.
        this->_prepare();

        this->_check();
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

        // Prepare the cryptographic contexts.
        this->_prepare();

        this->_check();
      }

      PublicKey::PublicKey(PublicKey const& other):
        _encryption_padding(other._encryption_padding),
        _signature_padding(other._signature_padding),
        _digest_algorithm(other._digest_algorithm),
        _envelope_cipher(other._envelope_cipher),
        _envelope_mode(other._envelope_mode)
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

        this->_prepare();

        this->_check();
      }

      PublicKey::PublicKey(PublicKey&& other):
        _key(std::move(other._key)),
        _encryption_padding(std::move(other._encryption_padding)),
        _signature_padding(std::move(other._signature_padding)),
        _digest_algorithm(std::move(other._digest_algorithm)),
        _envelope_cipher(std::move(other._envelope_cipher)),
        _envelope_mode(std::move(other._envelope_mode))
      {
        this->_context.encrypt = std::move(other._context.encrypt);
        this->_context.verify = std::move(other._context.verify);
#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        this->_context.rotate = std::move(other._context.rotate);
        this->_context.unrotate = std::move(other._context.unrotate);
#endif
        this->_context.envelope_padding_size =
          other._context.envelope_padding_size;

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
          throw Exception(
            elle::sprintf("unable to allocate the EVP_PKEY structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Set the rsa structure into the public key.
        if (::EVP_PKEY_assign_RSA(this->_key.get(), rsa) <= 0)
          throw Exception(
            elle::sprintf("unable to assign the RSA key to the EVP_PKEY "
                          "structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
      }

      void
      PublicKey::_prepare()
      {
        ELLE_DEBUG_FUNCTION("");

        ELLE_ASSERT_NEQ(this->_key, nullptr);

        // Prepare the encrypt context.
        ELLE_ASSERT_EQ(this->_context.encrypt, nullptr);
        this->_context.encrypt.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_encrypt_init,
                          this->_encryption_padding));

        this->_context.envelope_padding_size =
          padding::footprint(this->_context.encrypt.get());

        // Prepare the verify context.
        ELLE_ASSERT_EQ(this->_context.verify, nullptr);
        this->_context.verify.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_verify_init,
                          this->_signature_padding));

        if (this->_signature_padding == Padding::pss)
        {
          if (EVP_PKEY_CTX_set_rsa_pss_saltlen(this->_context.verify.get(),
                                               -2) <= 0)
            throw Exception(
              elle::sprintf("unable to set the EVP_PKEY context's PSS "
                            "salt length: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));
        }

        if (::EVP_PKEY_CTX_set_signature_md(
              this->_context.verify.get(),
              (void*)oneway::resolve(this->_digest_algorithm)) <= 0)
          throw Exception(
            elle::sprintf("unable to set the EVP_PKEY context's digest "
                          "function: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        // These contexts do not use paddings. Not that relying on textbook
        // RSA is considered foolish. In this case however, restricting the
        // rotation/derivation to content of the size of the RSA key's modulus
        // makes it secure.

        // Prepare the rotation context.
        ELLE_ASSERT_EQ(this->_context.rotate, nullptr);
        this->_context.rotate.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_encrypt_init,
                          Padding::none));

        // Prepare the unrotate context.
        ELLE_ASSERT_EQ(this->_context.unrotate, nullptr);
        this->_context.unrotate.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_verify_recover_init,
                          Padding::none));
#endif
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
      Code
      PublicKey::encrypt(Plain const& plain) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("plain: %x", plain);

        return (Code(evp::asymmetric::encrypt(
                       plain.buffer(),
                       this->_context.encrypt.get(),
                       ::EVP_PKEY_encrypt,
                       cipher::resolve(this->_envelope_cipher,
                                       this->_envelope_mode),
                       oneway::resolve(this->_digest_algorithm),
                       this->_context.envelope_padding_size)));
      }

      elle::Boolean
      PublicKey::verify(Signature const& signature,
                        Plain const& plain) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("signature: %x", signature);
        ELLE_DUMP("plain: %x", plain);

        Digest digest = hash(plain, this->_digest_algorithm);

        return (this->verify(signature, digest));
      }
#endif

      elle::Boolean
      PublicKey::verify(Signature const& signature,
                        Digest const& digest) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("signature: %x", signature);
        ELLE_DUMP("digest: %x", digest);

        return (evp::asymmetric::verify(signature.buffer(),
                                        digest.buffer(),
                                        this->_context.verify.get(),
                                        ::EVP_PKEY_verify));
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
          throw Exception(
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

        // Prepare the cryptographic contexts.
        this->_prepare();

        this->_check();
      }

      Seed
      PublicKey::rotate(Seed const& seed) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("seed: %x", seed);

        // Ensure the size of the seed equals the modulus.
        //
        // If the seed is too large, the algorithm would need to encrypt
        // it with a symmetric key etc. (as the encrypt() method does) which
        // would result in a future seed larger than the original.
        //
        // If it is too small, an attack could be performed against textbook
        // RSA which is the algorithm used in this case.
        if (seed.length() != this->length())
          throw Exception(
            elle::sprintf("unable to rotate a seed whose length does not match "
                          "the RSA key's modulus: %s versus %s",
                          seed.length(), this->length()));

        elle::Buffer buffer =
          evp::asymmetric::apply(seed.buffer(),
                                 this->_context.rotate.get(),
                                 ::EVP_PKEY_encrypt);

        // Make sure the seed does not grow over time.
        ELLE_ASSERT_EQ(seed.buffer().size(), buffer.size());

        return (Seed(std::move(buffer), seed.length()));
      }

      Seed
      PublicKey::unrotate(Seed const& seed) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("seed: %x", seed);

        // As for the rotation mechanism, ensure the size of the seed
        // equals the modulus.
        if (seed.length() != this->length())
          throw Exception(
            elle::sprintf("unable to unrotate a seed whose length does not "
                          "match the RSA key's modulus: %s versus %s",
                          seed.length(), this->length()));

        elle::Buffer buffer =
          evp::asymmetric::apply(seed.buffer(),
                                 this->_context.unrotate.get(),
                                 ::EVP_PKEY_verify_recover);

        // Make sure the unrotated seed has the same size as the original.
        ELLE_ASSERT_EQ(seed.buffer().size(), buffer.size());

        return (Seed(std::move(buffer), seed.length()));
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
          throw Exception(
            elle::sprintf("unable to set the EVP key's type: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        this->serialize(serializer);

        this->_prepare();
        this->_check();
      }

      void
      PublicKey::serialize(elle::serialization::Serializer& serializer)
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);

        cryptography::serialize<publickey::Serialization>(
          serializer,
          this->_key->pkey.rsa);

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
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(other._key, nullptr);
        return boost::lexical_cast<std::string>(*this)
          < boost::lexical_cast<std::string>(other);
      }
#endif
    }
  }
}
