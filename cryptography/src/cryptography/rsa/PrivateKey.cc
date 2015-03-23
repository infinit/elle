#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/Seed.hh>
#include <cryptography/rsa/padding.hh>
#include <cryptography/rsa/context.hh>
#include <cryptography/Seed.hh>
#include <cryptography/Code.hh>
#include <cryptography/Exception.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/bn.hh>
#include <cryptography/evp.hh>

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

      PrivateKey::PrivateKey(::EVP_PKEY* key):
        _key(key)
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

        // Prepare the cryptographic contexts.
        this->_prepare();

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

      PrivateKey::PrivateKey(::RSA* rsa)
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

      PrivateKey::PrivateKey(::BIGNUM* n,
                             ::BIGNUM* e,
                             ::BIGNUM* d,
                             ::BIGNUM* p,
                             ::BIGNUM* q,
                             ::BIGNUM* dmp1,
                             ::BIGNUM* dmq1,
                             ::BIGNUM* iqmp)
      {
        ELLE_ASSERT_NEQ(n, nullptr);
        ELLE_ASSERT_NEQ(e, nullptr);
        ELLE_ASSERT_NEQ(d, nullptr);
        ELLE_ASSERT_NEQ(p, nullptr);
        ELLE_ASSERT_NEQ(q, nullptr);
        ELLE_ASSERT_NEQ(dmp1, nullptr);
        ELLE_ASSERT_NEQ(dmq1, nullptr);
        ELLE_ASSERT_NEQ(iqmp, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Call a private method for constructing the object so as
        // to also be callable from the serialization mechanism, especially
        // the deserialization.
        this->_construct(n, e, d, p, q, dmp1, dmq1, iqmp);

        // Prepare the cryptographic contexts.
        this->_prepare();

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

      PrivateKey::PrivateKey(PrivateKey const& other):
        elle::serialize::DynamicFormat<PrivateKey>(other)
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

        // Call a private method for constructing the object so as
        // to also be callable from the serialization mechanism, especially
        // the deserialization.
        this->_construct(::BN_dup(other._key->pkey.rsa->n),
                         ::BN_dup(other._key->pkey.rsa->e),
                         ::BN_dup(other._key->pkey.rsa->d),
                         ::BN_dup(other._key->pkey.rsa->p),
                         ::BN_dup(other._key->pkey.rsa->q),
                         ::BN_dup(other._key->pkey.rsa->dmp1),
                         ::BN_dup(other._key->pkey.rsa->dmq1),
                         ::BN_dup(other._key->pkey.rsa->iqmp));

        // Prepare the cryptographic contexts.
        this->_prepare();

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

      PrivateKey::PrivateKey(PrivateKey&& other):
        elle::serialize::DynamicFormat<PrivateKey>(std::move(other)),
        _key(std::move(other._key)),
        _context_decrypt(std::move(other._context_decrypt)),
        _context_sign(std::move(other._context_sign)),
        _context_encrypt(std::move(other._context_encrypt)),
        _context_encrypt_padding_size(other._context_encrypt_padding_size)
#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        , _context_derive(std::move(other._context_derive))
        , _context_rotate(std::move(other._context_rotate))
#endif
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        ELLE_ASSERT_EQ(other._key, nullptr);
        ELLE_ASSERT_EQ(other._context_decrypt, nullptr);
        ELLE_ASSERT_EQ(other._context_sign, nullptr);
        ELLE_ASSERT_EQ(other._context_encrypt, nullptr);
#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        ELLE_ASSERT_EQ(other._context_derive, nullptr);
        ELLE_ASSERT_EQ(other._context_rotate, nullptr);
#endif
      }

      ELLE_SERIALIZE_CONSTRUCT_DEFINE(PrivateKey)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // XXX would need to prepare

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
      PrivateKey::_construct(::BIGNUM* n,
                             ::BIGNUM* e,
                             ::BIGNUM* d,
                             ::BIGNUM* p,
                             ::BIGNUM* q,
                             ::BIGNUM* dmp1,
                             ::BIGNUM* dmq1,
                             ::BIGNUM* iqmp)
      {
        ELLE_DEBUG_FUNCTION(n, e, d, p, q, dmp1, dmq1, iqmp);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(n);
        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(e);
        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(d);
        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(p);
        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(q);
        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(dmp1);
        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(dmq1);
        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(iqmp);

        ::RSA* rsa = nullptr;

        // Create the RSA structure.
        if ((rsa = ::RSA_new()) == nullptr)
          throw Exception(
            elle::sprintf("unable to allocate the RSA structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);

        // Assign the big numbers relevant to the private key.
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

        // Construct the private key based on an RSA key.
        this->_construct(rsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);
      }

      void
      PrivateKey::_prepare()
      {
        ELLE_DEBUG_FUNCTION("");

        ELLE_ASSERT_NEQ(this->_key, nullptr);

        // Depending on the format from which the private key has been
        // serialized or created, set the padding for the contexts.
        int padding_decrypt;
        int padding_sign;
        int padding_encrypt;

        switch (this->version())
        {
          case 0:
          {
            padding_decrypt = RSA_PKCS1_OAEP_PADDING;
            padding_sign = RSA_PKCS1_PADDING;
            padding_encrypt = RSA_PKCS1_PADDING;

            break;
          }
          case 1:
          {
            padding_decrypt = RSA_PKCS1_PADDING;
            padding_sign = RSA_PKCS1_PADDING;
            padding_encrypt = RSA_PKCS1_PADDING;

            break;
          }
          case 2:
          {
            padding_decrypt = RSA_PKCS1_OAEP_PADDING;
            padding_sign = RSA_PKCS1_PSS_PADDING;
            padding_encrypt = RSA_PKCS1_PADDING;

            break;
          }
          default:
            throw Exception(
              elle::sprintf("unknown format '%s'", this->version()));
        }

        // Prepare the decrypt context.
        ELLE_ASSERT_EQ(this->_context_decrypt, nullptr);
        this->_context_decrypt.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_decrypt_init,
                          padding_decrypt));

        // Prepare the sign context.
        ELLE_ASSERT_EQ(this->_context_sign, nullptr);
        this->_context_sign.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_sign_init,
                          padding_sign));

        switch (this->version())
        {
          case 0:
          case 1:
          {
            break;
          }
          case 2:
          {
            // Configure the PSS signature-specific padding.
            if (::EVP_PKEY_CTX_ctrl(this->_context_sign.get(),
                                    EVP_PKEY_RSA,
                                    -1,
                                    EVP_PKEY_CTRL_RSA_PSS_SALTLEN,
                                    -2,
                                    nullptr) <= 0)
              throw Exception(
                elle::sprintf("unable to set the EVP_PKEY context's PSS "
                              "salt length: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            // Set the digest function.
            if (::EVP_PKEY_CTX_ctrl(this->_context_sign.get(),
                                    EVP_PKEY_RSA,
                                    EVP_PKEY_OP_TYPE_SIG,
                                    EVP_PKEY_CTRL_MD,
                                    0,
                                    (void*)::EVP_sha256()) <= 0)
              throw Exception(
                elle::sprintf("unable to set the EVP_PKEY context's digest "
                              "function: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            break;
          }
          default:
            throw Exception(
              elle::sprintf("unknown format '%s'", this->version()));
        }

        // Prepare the encrypt context.
        ELLE_ASSERT_EQ(this->_context_encrypt, nullptr);
        this->_context_encrypt.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_sign_init,
                          padding_encrypt));

        this->_context_encrypt_padding_size =
          padding::footprint(this->_context_encrypt.get());

        switch (this->version())
        {
          case 0:
          case 1:
          {
            break;
          }
          case 2:
          {
            // Set the digest function.
            /* XXX
            if (::EVP_PKEY_CTX_ctrl(this->_context_encrypt.get(),
                                    EVP_PKEY_RSA,
                                    EVP_PKEY_OP_TYPE_SIG,
                                    EVP_PKEY_CTRL_MD,
                                    0,
                                    (void*)::EVP_sha256()) <= 0)
              throw Exception(
                elle::sprintf("unable to set the EVP_PKEY context's digest "
                              "function: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));
            */
            break;
          }
          default:
            throw Exception(
              elle::sprintf("unknown format '%s'", this->version()));
        }

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        // Note that in these cases, using no RSA padding is not dangerous
        // because (1) the content being rotated is always random i.e cannot be
        // guessed because produced by a human being (2) the content is always
        // the size of the RSA key's modulus.

        // Prepare the derive context.
        ELLE_ASSERT_EQ(this->_context_derive, nullptr);
        this->_context_derive.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_decrypt_init,
                          RSA_NO_PADDING));

        // Prepare the rotate context.
        ELLE_ASSERT_EQ(this->_context_rotate, nullptr);
        this->_context_rotate.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_sign_init,
                          RSA_NO_PADDING));
#endif
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

        // Compare the internal numbers.
        return ((::BN_cmp(this->_key->pkey.rsa->n,
                          other._key->pkey.rsa->n) == 0) &&
                (::BN_cmp(this->_key->pkey.rsa->d,
                          other._key->pkey.rsa->d) == 0) &&
                (::BN_cmp(this->_key->pkey.rsa->p,
                          other._key->pkey.rsa->p) == 0) &&
                (::BN_cmp(this->_key->pkey.rsa->q,
                          other._key->pkey.rsa->q) == 0));
      }

      elle::Boolean
      PrivateKey::operator <(PrivateKey const& other) const
      {
        if (this == &other)
          return (true);

        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(other._key, nullptr);

        // Compare the internal numbers.
        int cmp_n = ::BN_cmp(this->_key->pkey.rsa->n,
                             other._key->pkey.rsa->n);

        if (cmp_n < 0)
          return (true);
        else if (cmp_n > 0)
          return (false);

        int cmp_e = ::BN_cmp(this->_key->pkey.rsa->e,
                             other._key->pkey.rsa->e);

        if (cmp_e < 0)
          return (true);
        else
          return (false);

        int cmp_d = ::BN_cmp(this->_key->pkey.rsa->d,
                             other._key->pkey.rsa->d);

        if (cmp_d < 0)
          return (true);
        else
          return (false);

        int cmp_p = ::BN_cmp(this->_key->pkey.rsa->p,
                             other._key->pkey.rsa->p);

        if (cmp_p < 0)
          return (true);
        else
          return (false);

        int cmp_q = ::BN_cmp(this->_key->pkey.rsa->q,
                             other._key->pkey.rsa->q);

        if (cmp_q < 0)
          return (true);
        else
          return (false);

        int cmp_dmp1 = ::BN_cmp(this->_key->pkey.rsa->dmp1,
                                other._key->pkey.rsa->dmp1);

        if (cmp_dmp1 < 0)
          return (true);
        else
          return (false);

        int cmp_dmq1 = ::BN_cmp(this->_key->pkey.rsa->dmq1,
                                other._key->pkey.rsa->dmq1);

        if (cmp_dmq1 < 0)
          return (true);
        else
          return (false);

        int cmp_iqmp = ::BN_cmp(this->_key->pkey.rsa->iqmp,
                                other._key->pkey.rsa->iqmp);

        if (cmp_iqmp < 0)
          return (true);
        else
          return (false);

        elle::unreachable();
      }

      /*------------.
      | Private Key |
      `------------*/

      elle::Boolean
      PrivateKey::operator ==(
        cryptography::privatekey::Interface const& other) const
      {
        if (this == &other)
          return (true);

        if (this->cryptosystem() != other.cryptosystem())
          return (false);

        return (*this == static_cast<PrivateKey const&>(other));
      }

      elle::Boolean
      PrivateKey::operator <(
        cryptography::privatekey::Interface const& other) const
      {
        if (this == &other)
          return (true);

        if (this->cryptosystem() != other.cryptosystem())
          return (this->cryptosystem() < other.cryptosystem());

        return (*this < static_cast<PrivateKey const&>(other));
      }

      cryptography::privatekey::Interface*
      PrivateKey::clone() const
      {
        return (new PrivateKey(*this));
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

      Cryptosystem
      PrivateKey::cryptosystem() const
      {
        return (Cryptosystem::rsa);
      }

      Clear
      PrivateKey::decrypt(Code const& code) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("code: %x", code);

        return (evp::asymmetric::decrypt(code,
                                         this->_context_decrypt.get(),
                                         ::EVP_PKEY_decrypt));
      }

      Signature
      PrivateKey::sign(Digest const& digest) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("digest: %x", digest);

        return (evp::asymmetric::sign(digest,
                                      this->_context_sign.get(),
                                      ::EVP_PKEY_sign));
      }

      // Since the private key size limits the size of the data that can be
      // encrypted and raising large data to large exponent is very slow;
      // the algorithm below consists in (1) generating a secret key (2)
      // ciphering the plain text with this key (3) encrypting the secret
      // key with the private key and finally (4) returning an archive
      // containing the asymetrically-encrypted secret key with the
      // symmetrically-encrypted data.
      Code
      PrivateKey::encrypt(Plain const& plain) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("plain: %x", plain);

        return (evp::asymmetric::encrypt(plain,
                                         this->_context_encrypt.get(),
                                         ::EVP_PKEY_sign,
                                         this->_context_encrypt_padding_size));
      }

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
      cryptography::Seed
      PrivateKey::derive(cryptography::Seed const& seed) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("seed: %x", seed);

        ELLE_ASSERT_EQ(seed.cryptosystem(), Cryptosystem::rsa);

        // Cast the seed into an actual RSA seed.
        ELLE_ASSERT_NEQ(dynamic_cast<Seed const*>(&seed.implementation()),
                        nullptr);
        Seed const& _seed = static_cast<Seed const&>(seed.implementation());

        // As for the rotation mechanism, ensure the size of the seed
        // equals the modulus.
        if (_seed.length() != this->length())
          throw Exception(
            elle::sprintf("unable to derive a seed whose length does not match "
                          "the RSA key's modulus: %s versus %s",
                          _seed.length(), this->length()));

        elle::Buffer buffer =
          evp::asymmetric::apply(elle::WeakBuffer{_seed.buffer()},
                                 this->_context_derive.get(),
                                 ::EVP_PKEY_decrypt);

        // Make sure the derived seed has the same size as the original.
        ELLE_ASSERT_EQ(_seed.buffer().size(), buffer.size());

        // Create an implementation of an RSA seed.
        std::unique_ptr<cryptography::seed::Interface> implementation(
          new Seed(std::move(buffer), _seed.length()));

        return (cryptography::Seed(std::move(implementation)));
      }

      cryptography::Seed
      PrivateKey::rotate(cryptography::Seed const& seed) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("seed: %x", seed);

        ELLE_ASSERT_EQ(seed.cryptosystem(), Cryptosystem::rsa);

        // Cast the seed into an actual RSA seed.
        ELLE_ASSERT_NEQ(dynamic_cast<Seed const*>(&seed.implementation()),
                        nullptr);
        Seed const& _seed = static_cast<Seed const&>(seed.implementation());

        // Ensure the size of the seed equals the modulus.
        //
        // If the seed is too large, the algorithm would need to encrypt
        // it with a symmetric key etc. (as the encrypt() method does) which
        // would result in a future seed larger than the original.
        //
        // If it is too small, an attack could be performed against textbook
        // RSA which is the algorithm used in this case.
        if (_seed.length() != this->length())
          throw Exception(
            elle::sprintf("unable to rotate a seed whose length does not match "
                          "the RSA key's modulus: %s versus %s",
                          _seed.length(), this->length()));

        elle::Buffer buffer =
          evp::asymmetric::apply(elle::WeakBuffer{_seed.buffer()},
                                 this->_context_rotate.get(),
                                 ::EVP_PKEY_sign);

        // Make sure the seed does not grow over time.
        ELLE_ASSERT_EQ(_seed.buffer().size(), buffer.size());

        // Create an implementation of an RSA seed.
        std::unique_ptr<cryptography::seed::Interface> implementation(
          new Seed(std::move(buffer), _seed.length()));

        return (cryptography::Seed(std::move(implementation)));
      }
#endif

      /*--------------.
      | Serialization |
      `--------------*/

      PrivateKey::PrivateKey(elle::serialization::SerializerIn& serializer)
        : _key(::EVP_PKEY_new())
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
      static const elle::serialization::Hierarchy
      <cryptography::privatekey::Interface>::
      Register<PrivateKey> _register_("rsa");

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

        stream << "[";
        padding::print(stream, this->_context_decrypt.get());
        stream << ", ";
        padding::print(stream, this->_context_sign.get());
        stream << ", ";
        padding::print(stream, this->_context_encrypt.get());
#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        stream << ", ";
        padding::print(stream, this->_context_derive.get());
        stream << ", ";
        padding::print(stream, this->_context_rotate.get());
#endif
        stream << "]";
      }
    }
  }
}

//
// ---------- Generator -------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace privatekey
      {
#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        PrivateKey
        deduce(cryptography::seed::Interface const& seed)
        {
          ELLE_TRACE_FUNCTION("");
          ELLE_DUMP("seed: %x", seed);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          ELLE_ASSERT_EQ(seed.cryptosystem(), Cryptosystem::rsa);

          // Cast the interface into an actual RSA seed.
          ELLE_ASSERT_NEQ(dynamic_cast<Seed const*>(&seed), nullptr);
          Seed const& _seed = static_cast<Seed const&>(seed);

          // Deduce the RSA key from the given seed.
          ::RSA* rsa = nullptr;

          if ((rsa = ::dRSA_deduce_privatekey(
                 _seed.length(),
                 static_cast<unsigned char const*>(_seed.buffer().contents()),
                 _seed.buffer().size())) == nullptr)
            throw Exception(
              elle::sprintf("unable to deduce the RSA key from the given "
                            "seed: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);

          // Instanciate an RSA private key by transferring the ownership
          // of the RSA structure.
          PrivateKey k(rsa);

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);

          return (k);
        }
#endif

        PrivateKey
        construct(::EVP_PKEY* key)
        {
          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

          // Instanciate an RSA private key by transferring the ownership
          // of the EVP structure.
          PrivateKey k(key);

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(key);

          return (k);
        }
      }
    }
  }
}
