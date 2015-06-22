#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/rsa/Seed.hh>
#include <cryptography/rsa/Padding.hh>
#include <cryptography/rsa/context.hh>
#include <cryptography/Code.hh>
#include <cryptography/Exception.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/bn.hh>
#include <cryptography/evp.hh>
#include <cryptography/low.hh>
#include <cryptography/hash.hh>

#include <elle/log.hh>

#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

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
      /*-------------.
      | Construction |
      `-------------*/

      PrivateKey::PrivateKey()
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();
      }

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
          throw Exception(
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

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
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
          throw Exception(
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
#endif

      PrivateKey::PrivateKey(PrivateKey const& other):
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
        _key(std::move(other._key)),
        _encryption_padding(std::move(other._encryption_padding)),
        _signature_padding(std::move(other._signature_padding)),
        _digest_algorithm(std::move(other._digest_algorithm))
      {
        this->_context.decrypt = std::move(other._context.decrypt);
        this->_context.sign = std::move(other._context.sign);
#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        this->_context.unrotate = std::move(other._context.unrotate);
        this->_context.rotate = std::move(other._context.rotate);
#endif

        // Make sure the cryptographic system is set up.
        cryptography::require();

        this->_check();
      }

      ELLE_SERIALIZE_CONSTRUCT_DEFINE(PrivateKey)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();
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
          throw Exception(
            elle::sprintf("unable to allocate the EVP_PKEY structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Set the rsa structure into the private key.
        if (::EVP_PKEY_assign_RSA(this->_key.get(), rsa) <= 0)
          throw Exception(
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
            throw Exception(
              elle::sprintf("unable to set the EVP_PKEY context's PSS "
                            "salt length: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));
        }

        if (::EVP_PKEY_CTX_set_signature_md(
              this->_context.sign.get(),
              (void*)oneway::resolve(this->_digest_algorithm)) <= 0)
          throw Exception(
            elle::sprintf("unable to set the EVP_PKEY context's digest "
                          "function: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        // Note that in these cases, using no RSA padding is not dangerous
        // because (1) the content being rotated is always random i.e cannot be
        // guessed because produced by a human being (2) the content is always
        // the size of the RSA key's modulus.

        // Prepare the unrotate context.
        ELLE_ASSERT_EQ(this->_context.unrotate, nullptr);
        this->_context.unrotate.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_decrypt_init,
                          Padding::none));

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

      Signature
      PrivateKey::sign(Digest const& digest) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("digest: %x", digest);

        return (Signature(evp::asymmetric::sign(digest.buffer(),
                                                this->_context.sign.get(),
                                                ::EVP_PKEY_sign)));
      }

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
      Seed
      PrivateKey::unrotate(Seed const& seed) const
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
                                 ::EVP_PKEY_decrypt);

        // Make sure the unrotated seed has the same size as the original.
        ELLE_ASSERT_EQ(seed.buffer().size(), buffer.size());

        return (Seed(std::move(buffer), seed.length()));
      }

      Seed
      PrivateKey::rotate(Seed const& seed) const
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
                                 ::EVP_PKEY_sign);

        // Make sure the seed does not grow over time.
        ELLE_ASSERT_EQ(seed.buffer().size(), buffer.size());

        return (Seed(std::move(buffer), seed.length()));
      }
#endif

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

      PrivateKey::PrivateKey(elle::serialization::SerializerIn& serializer):
        _key(::EVP_PKEY_new())
      {
        std::unique_ptr<RSA, void (*) (::RSA*)> rsa(::RSA_new(), &::RSA_free);
        if (!rsa)
          throw elle::Error(
            elle::sprintf("unable to initialize RSA: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
        if (::EVP_PKEY_assign_RSA(this->_key.get(), rsa.get()) <= 0)
          throw elle::Error(
            elle::sprintf("unable to assign the RSA: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
        rsa.release();
        this->serialize(serializer);
        this->_prepare();
      }

      void
      PrivateKey::serialize(elle::serialization::Serializer& serializer)
      {
        serializer.serialize("modulus", this->_key->pkey.rsa->n);
        serializer.serialize("public_exponent", this->_key->pkey.rsa->e);
        serializer.serialize("private_exponent", this->_key->pkey.rsa->d);
        // FIXME: https://www.openssl.org/docs/crypto/rsa.html
        // p, q, dmp1, dmq1 and iqmp may be NULL in private keys, but the RSA
        // operations are much faster when these values are available.
        // serializer.serialize("p", this->_key->pkey.rsa->p);
        // serializer.serialize("q", this->_key->pkey.rsa->q);
        // serializer.serialize("dmp1", this->_key->pkey.rsa->dmp1);
        // serializer.serialize("dmq1", this->_key->pkey.rsa->dmq1);
        // serializer.serialize("iqmp;", this->_key->pkey.rsa->iqmp);
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
