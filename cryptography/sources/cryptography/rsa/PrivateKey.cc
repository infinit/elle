#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/Seed.hh>
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

#if defined(ELLE_CRYPTOGRAPHY_ROTATION)
# include <dopenssl/rsa.h>
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
        PrivateKey(::BN_dup(other._key->pkey.rsa->n),
                   ::BN_dup(other._key->pkey.rsa->e),
                   ::BN_dup(other._key->pkey.rsa->d),
                   ::BN_dup(other._key->pkey.rsa->p),
                   ::BN_dup(other._key->pkey.rsa->q),
                   ::BN_dup(other._key->pkey.rsa->dmp1),
                   ::BN_dup(other._key->pkey.rsa->dmq1),
                   ::BN_dup(other._key->pkey.rsa->iqmp))
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();
      }

      PrivateKey::PrivateKey(PrivateKey&& other):
        _key(std::move(other._key)),
        _context_decrypt(std::move(other._context_decrypt)),
        _context_sign(std::move(other._context_sign)),
        _context_encrypt(std::move(other._context_encrypt)),
        _context_rotate(std::move(other._context_rotate))
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        ELLE_ASSERT_EQ(other._key, nullptr);
        ELLE_ASSERT_EQ(other._context_decrypt, nullptr);
        ELLE_ASSERT_EQ(other._context_sign, nullptr);
        ELLE_ASSERT_EQ(other._context_encrypt, nullptr);
        ELLE_ASSERT_EQ(other._context_rotate, nullptr);
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

        // Prepare the decrypt context.
        ELLE_ASSERT_EQ(this->_context_decrypt, nullptr);
        this->_context_decrypt.reset(
          ::EVP_PKEY_CTX_new(this->_key.get(), nullptr));

        if (this->_context_decrypt == nullptr)
          throw Exception(
            elle::sprintf("unable to allocate a EVP_PKEY context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        if (::EVP_PKEY_decrypt_init(this->_context_decrypt.get()) <= 0)
          throw Exception(
            elle::sprintf("unable to initialize the EVP_PKEY context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        if (::EVP_PKEY_CTX_ctrl(this->_context_decrypt.get(),
                                EVP_PKEY_RSA,
                                -1,
                                EVP_PKEY_CTRL_RSA_PADDING,
                                RSA_PKCS1_OAEP_PADDING,
                                nullptr) <= 0)
          throw Exception(
            elle::sprintf("unable to control the EVP_PKEY context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Prepare the sign context.
        ELLE_ASSERT_EQ(this->_context_sign, nullptr);
        this->_context_sign.reset(
          ::EVP_PKEY_CTX_new(this->_key.get(), nullptr));

        if (this->_context_sign == nullptr)
          throw Exception(
            elle::sprintf("unable to allocate a EVP_PKEY context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        if (::EVP_PKEY_sign_init(this->_context_sign.get()) <= 0)
          throw Exception(
            elle::sprintf("unable to initialize the EVP_PKEY context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        if (::EVP_PKEY_CTX_ctrl(this->_context_sign.get(),
                                EVP_PKEY_RSA,
                                -1,
                                EVP_PKEY_CTRL_RSA_PADDING,
                                RSA_PKCS1_PADDING,
                                nullptr) <= 0)
          throw Exception(
            elle::sprintf("unable to control the EVP_PKEY context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Prepare the encrypt context.
        ELLE_ASSERT_EQ(this->_context_encrypt, nullptr);
        this->_context_encrypt.reset(
          ::EVP_PKEY_CTX_new(this->_key.get(), nullptr));

        if (this->_context_encrypt == nullptr)
          throw Exception(
            elle::sprintf("unable to allocate a EVP_PKEY context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        if (::EVP_PKEY_sign_init(this->_context_encrypt.get()) <= 0)
          throw Exception(
            elle::sprintf("unable to initialize the EVP_PKEY context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        if (::EVP_PKEY_CTX_ctrl(this->_context_encrypt.get(),
                                EVP_PKEY_RSA,
                                -1,
                                EVP_PKEY_CTRL_RSA_PADDING,
                                RSA_PKCS1_PADDING,
                                nullptr) <= 0)
          throw Exception(
            elle::sprintf("unable to control the EVP_PKEY context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Prepare the rotate context.
        //
        // Note that in this case, using no RSA padding is not dangerous because
        // (1) the content being rotated is always random i.e cannot be guessed
        // because produced by a human being (2) the content is always the size
        // of the RSA key's modulus.
        ELLE_ASSERT_EQ(this->_context_rotate, nullptr);
        this->_context_rotate.reset(
          ::EVP_PKEY_CTX_new(this->_key.get(), nullptr));

        if (this->_context_rotate == nullptr)
          throw Exception(
            elle::sprintf("unable to allocate a EVP_PKEY context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        if (::EVP_PKEY_sign_init(this->_context_rotate.get()) <= 0)
          throw Exception(
            elle::sprintf("unable to initialize the EVP_PKEY context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        if (::EVP_PKEY_CTX_ctrl(this->_context_rotate.get(),
                                EVP_PKEY_RSA,
                                -1,
                                EVP_PKEY_CTRL_RSA_PADDING,
                                RSA_NO_PADDING,
                                nullptr) <= 0)
          throw Exception(
            elle::sprintf("unable to control the EVP_PKEY context: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
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
        ELLE_TRACE_METHOD(code);

        return (evp::asymmetric::decrypt(code,
                                         this->_context_decrypt.get(),
                                         ::EVP_PKEY_decrypt));
      }

      Signature
      PrivateKey::sign(Plain const& plain) const
      {
        ELLE_TRACE_METHOD(plain);

        return (evp::asymmetric::sign(plain,
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
        ELLE_TRACE_METHOD(plain);

        return (evp::asymmetric::encrypt(plain,
                                         this->_context_encrypt.get(),
                                         ::EVP_PKEY_sign));
      }

#if defined(ELLE_CRYPTOGRAPHY_ROTATION)
      cryptography::Seed
      PrivateKey::rotate(cryptography::Seed const& seed) const
      {
        ELLE_TRACE_METHOD(seed);

        ELLE_ASSERT_EQ(seed.cryptosystem(), Cryptosystem::rsa);

        // Cast the seed into an actual RSA seed.
        ELLE_ASSERT_NEQ(dynamic_cast<Seed const*>(&seed.implementation()),
                        nullptr);
        Seed const& _seed = static_cast<Seed const&>(seed.implementation());

        // Ensure the size of the seed equals the modulus.
        //
        // If the seed is too large, the algorithm would need to encrypt
        // it with a symmetric keg etc. (as the encrypt() method does) which
        // would result in a seed larger than the original.
        //
        // If it is too small, an attack could be performed against textbook
        // RSA which is the algorithm used in this case.
        if (_seed.buffer().size() !=
            static_cast<elle::Natural32>(::EVP_PKEY_size(this->_key.get())))
          throw Exception("unable to rotate a seed whose size does not match "
                          "the RSA key's modulus");

        elle::Buffer buffer =
          evp::asymmetric::apply(elle::WeakBuffer{_seed.buffer()},
                                 this->_context_rotate.get(),
                                 ::EVP_PKEY_sign);

        // Make sure the seed does not grow over time.
        ELLE_ASSERT_EQ(_seed.buffer().size(), buffer.size());

        // Create an implementation of an RSA seed.
        std::unique_ptr<cryptography::seed::Interface> implementation(
          new Seed(std::move(buffer), ::BN_dup(_seed.n())));

        return (cryptography::Seed(std::move(implementation)));
      }
#endif

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
#if defined(ELLE_CRYPTOGRAPHY_ROTATION)
        PrivateKey
        generate(cryptography::seed::Interface const& seed)
        {
          ELLE_TRACE_FUNCTION(seed);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          ELLE_ASSERT_EQ(seed.cryptosystem(), Cryptosystem::rsa);

          // Cast the interface into an actual RSA seed.
          ELLE_ASSERT_NEQ(dynamic_cast<Seed const*>(&seed), nullptr);
          Seed const& _seed = static_cast<Seed const&>(seed);

          ELLE_ASSERT_EQ(_seed.buffer().size(),
                         static_cast<elle::Natural32>(BN_num_bytes(_seed.n())));

          // Deduce the RSA key from the given seed.
          ::RSA* rsa = nullptr;

          if ((rsa = ::dRSA_deduce_privatekey(
                 ::BN_num_bits(_seed.n()),
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

          return (PrivateKey(std::move(k)));
        }
#endif
      }
    }
  }
}
