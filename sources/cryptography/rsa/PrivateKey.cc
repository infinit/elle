#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/Seed.hh>
#include <cryptography/Code.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/bn.hh>
#include <cryptography/evp.hh>

#include <elle/Exception.hh>
#include <elle/log.hh>

// XXX #include <comet/Comet.hh>

#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

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

      PrivateKey::PrivateKey():
        _key(nullptr),
        _context_decrypt(nullptr),
        _context_sign(nullptr),
        _context_encrypt(nullptr)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();
      }

      PrivateKey::PrivateKey(::EVP_PKEY* key):
        _key(key),
        _context_decrypt(nullptr),
        _context_sign(nullptr),
        _context_encrypt(nullptr)
      {
        ELLE_ASSERT(key != nullptr);
        ELLE_ASSERT(key->pkey.rsa != nullptr);
        ELLE_ASSERT(key->pkey.rsa->n != nullptr);
        ELLE_ASSERT(key->pkey.rsa->e != nullptr);
        ELLE_ASSERT(key->pkey.rsa->d != nullptr);
        ELLE_ASSERT(key->pkey.rsa->p != nullptr);
        ELLE_ASSERT(key->pkey.rsa->q != nullptr);
        ELLE_ASSERT(key->pkey.rsa->dmp1 != nullptr);
        ELLE_ASSERT(key->pkey.rsa->dmq1 != nullptr);
        ELLE_ASSERT(key->pkey.rsa->iqmp != nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Prepare the cryptographic contexts.
        this->_prepare();

        ELLE_ASSERT(this->_key != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->n != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->e != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->d != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->p != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->q != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->dmp1 != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->dmq1 != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->iqmp != nullptr);
      }

      PrivateKey::PrivateKey(::BIGNUM* n,
                             ::BIGNUM* e,
                             ::BIGNUM* d,
                             ::BIGNUM* p,
                             ::BIGNUM* q,
                             ::BIGNUM* dmp1,
                             ::BIGNUM* dmq1,
                             ::BIGNUM* iqmp):
        _key(nullptr),
        _context_decrypt(nullptr),
        _context_sign(nullptr),
        _context_encrypt(nullptr)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Call a private method for constructing the object so as
        // to also be callable from the serialization mechanism, especially
        // the deserialization.
        this->_construct(n, e, d, p, q, dmp1, dmq1, iqmp);

        // Prepare the cryptographic contexts.
        this->_prepare();

        ELLE_ASSERT(this->_key != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->n != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->e != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->d != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->p != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->q != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->dmp1 != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->dmq1 != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->iqmp != nullptr);
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

        ELLE_ASSERT(this->_key != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->n != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->e != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->d != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->p != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->q != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->dmp1 != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->dmq1 != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->iqmp != nullptr);
      }

      PrivateKey::PrivateKey(PrivateKey&& other):
        PrivateKey(other._key)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Reset the pointer for the given key.
        other._key = nullptr;

        ELLE_ASSERT(this->_key != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->n != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->e != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->d != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->p != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->q != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->dmp1 != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->dmq1 != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->iqmp != nullptr);
      }

      ELLE_SERIALIZE_CONSTRUCT_DEFINE(PrivateKey)
      {
        this->_key = nullptr;
        this->_context_decrypt = nullptr;
        this->_context_sign = nullptr;
        this->_context_encrypt = nullptr;

        // Make sure the cryptographic system is set up.
        cryptography::require();
      }

      PrivateKey::~PrivateKey()
      {
        if (this->_key != nullptr)
          ::EVP_PKEY_free(this->_key);

        if (this->_context_decrypt != nullptr)
          ::EVP_PKEY_CTX_free(this->_context_decrypt);

        if (this->_context_sign != nullptr)
          ::EVP_PKEY_CTX_free(this->_context_sign);

        if (this->_context_encrypt != nullptr)
          ::EVP_PKEY_CTX_free(this->_context_encrypt);
      }

      /*--------.
      | Methods |
      `--------*/

      /* XXX
     ///
     /// this method derives a public key according to (i) its complementary
     /// private key and (ii) the seed used for rotating this key pair.
     ///
     elle::Status              PrivateKey::Derive(const Seed&          seed,
     PublicKey&           K) const
     {
     ELLE_TRACE_METHOD(seed, K);

     struct Scope
     {
     ::EVP_PKEY*       key;
     ::RSA*            rsa;

     Scope() : key(nullptr), rsa(nullptr) {}
     ~Scope() { ::EVP_PKEY_free(this->key); ::RSA_free(this->rsa); }
     } scope;

     // create an EVP key.
     if ((scope.key = ::EVP_PKEY_new()) == nullptr)
     throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));

     // create a new RSA key.
     if ((scope.rsa = ::RSA_new()) == nullptr)
     throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));

     // derive the RSA key.
     if (comet::RSA_derive(scope.rsa,
     this->_key->pkey.rsa->n,
     seed.region.contents,
     seed.region.size) <= 0)
     throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));

     // assign the RSA key to the EVP's.
     if (::EVP_PKEY_assign_RSA(scope.key, scope.rsa) <= 0)
     throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));

     // stop tracking.
     scope.rsa = nullptr;

     // create the rotated public key according to the EVP structure.
     if (K.Create(scope.key) == elle::Status::Error)
     throw elle::Exception("unable to create the public key");

     return elle::Status::Ok;
     }
      */

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

        // Initialise the private key structure.
        if ((this->_key = ::EVP_PKEY_new()) == nullptr)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        ::RSA* rsa;

        // Create the RSA structure.
        if ((rsa = ::RSA_new()) == nullptr)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

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

        // Set the rsa structure into the private key.
        if (::EVP_PKEY_assign_RSA(this->_key, rsa) <= 0)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);
      }

      void
      PrivateKey::_prepare()
      {
        ELLE_DEBUG_FUNCTION("");

        // Prepare the decrypt context.
        if ((this->_context_decrypt =
             ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        if (::EVP_PKEY_decrypt_init(this->_context_decrypt) <= 0)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        if (::EVP_PKEY_CTX_ctrl(this->_context_decrypt,
                                EVP_PKEY_RSA,
                                -1,
                                EVP_PKEY_CTRL_RSA_PADDING,
                                RSA_PKCS1_OAEP_PADDING,
                                nullptr) <= 0)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        // Prepare the sign context.
        if ((this->_context_sign =
             ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        if (::EVP_PKEY_sign_init(this->_context_sign) <= 0)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        if (::EVP_PKEY_CTX_ctrl(this->_context_sign,
                                EVP_PKEY_RSA,
                                -1,
                                EVP_PKEY_CTRL_RSA_PADDING,
                                RSA_PKCS1_PADDING,
                                nullptr) <= 0)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        // Prepare the encrypt context.
        if ((this->_context_encrypt =
             ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        if (::EVP_PKEY_sign_init(this->_context_encrypt) <= 0)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));

        if (::EVP_PKEY_CTX_ctrl(this->_context_encrypt,
                                EVP_PKEY_RSA,
                                -1,
                                EVP_PKEY_CTRL_RSA_PADDING,
                                RSA_PKCS1_PADDING,
                                nullptr) <= 0)
          throw elle::Exception("%s",
                                ::ERR_error_string(ERR_get_error(), nullptr));
      }

      /*----------.
      | Operators |
      `----------*/

      elle::Boolean
      PrivateKey::operator ==(PrivateKey const& other) const
      {
        if (this == &other)
          return (true);

        ELLE_ASSERT(this->_key != nullptr);
        ELLE_ASSERT(other._key != nullptr);

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

        ELLE_ASSERT(this->_key != nullptr);
        ELLE_ASSERT(other._key != nullptr);

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
      PrivateKey::operator ==(privatekey::Interface const& other) const
      {
        if (this == &other)
          return (true);

        if (this->cryptosystem() != other.cryptosystem())
          return (false);

        return (*this == static_cast<PrivateKey const&>(other));
      }

      elle::Boolean
      PrivateKey::operator <(privatekey::Interface const& other) const
      {
        if (this == &other)
          return (true);

        if (this->cryptosystem() != other.cryptosystem())
          return (this->cryptosystem() < other.cryptosystem());

        return (*this < static_cast<PrivateKey const&>(other));
      }

      privatekey::Interface*
      PrivateKey::clone() const
      {
        return (new PrivateKey{*this});
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
                                         this->_context_decrypt,
                                         ::EVP_PKEY_decrypt));
      }

      Signature
      PrivateKey::sign(Plain const& plain) const
      {
        ELLE_TRACE_METHOD(plain);

        return (evp::asymmetric::sign(plain,
                                      this->_context_sign,
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
                                         this->_context_encrypt,
                                         ::EVP_PKEY_sign));
      }

      /*----------.
      | Printable |
      `----------*/

      void
      PrivateKey::print(std::ostream& stream) const
      {
        ELLE_ASSERT(this->_key != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->n != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->e != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->d != nullptr);

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
