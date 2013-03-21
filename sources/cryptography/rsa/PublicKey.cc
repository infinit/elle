#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/Digest.hh>
#include <cryptography/Code.hh>
#include <cryptography/Exception.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/bn.hh>
#include <cryptography/evp.hh>

#include <elle/log.hh>

#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

ELLE_LOG_COMPONENT("infinit.cryptography.rsa.PublicKey");

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /*-------------.
      | Construction |
      `-------------*/

      PublicKey::PublicKey():
        _key(nullptr),
        _context_encrypt(nullptr),
        _context_verify(nullptr),
        _context_decrypt(nullptr)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();
      }

      PublicKey::PublicKey(::EVP_PKEY* key):
        _key(key),
        _context_encrypt(nullptr),
        _context_verify(nullptr),
        _context_decrypt(nullptr)
      {
        ELLE_ASSERT(key != nullptr);
        ELLE_ASSERT(key->pkey.rsa != nullptr);
        ELLE_ASSERT(key->pkey.rsa->n != nullptr);
        ELLE_ASSERT(key->pkey.rsa->e != nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Prepare the cryptographic contexts.
        this->_prepare();

        ELLE_ASSERT(this->_key != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->n != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->e != nullptr);
      }

      PublicKey::PublicKey(::BIGNUM* n,
                           ::BIGNUM* e):
        _key(nullptr),
        _context_encrypt(nullptr),
        _context_verify(nullptr),
        _context_decrypt(nullptr)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Call a private method for constructing the object so as
        // to also be callable from the serialization mechanism, especially
        // the deserialization.
        this->_construct(n, e);

        // Prepare the cryptographic contexts.
        this->_prepare();

        ELLE_ASSERT(this->_key != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->n != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->e != nullptr);
      }

      PublicKey::PublicKey(PublicKey const& other):
        PublicKey(::BN_dup(other._key->pkey.rsa->n),
                  ::BN_dup(other._key->pkey.rsa->e))
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        ELLE_ASSERT(this->_key != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->n != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->e != nullptr);
      }

      PublicKey::PublicKey(PublicKey&& other):
        PublicKey(other._key)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        ELLE_ASSERT(this->_key != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->n != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->e != nullptr);

        // Reset the pointer for the given key.
        other._key = nullptr;
      }

      ELLE_SERIALIZE_CONSTRUCT_DEFINE(PublicKey)
      {
        this->_key = nullptr;
        this->_context_encrypt = nullptr;
        this->_context_verify = nullptr;
        this->_context_decrypt = nullptr;

        // Make sure the cryptographic system is set up.
        cryptography::require();
      }

      PublicKey::~PublicKey()
      {
        if (this->_key != nullptr)
          ::EVP_PKEY_free(this->_key);

        if (this->_context_encrypt != nullptr)
          ::EVP_PKEY_CTX_free(this->_context_encrypt);

        if (this->_context_verify != nullptr)
          ::EVP_PKEY_CTX_free(this->_context_verify);

        if (this->_context_decrypt != nullptr)
          ::EVP_PKEY_CTX_free(this->_context_decrypt);
      }

      /*--------.
      | Methods |
      `--------*/

      void
      PublicKey::_construct(::BIGNUM* n,
                            ::BIGNUM* e)
      {
        ELLE_DEBUG_FUNCTION(n, e);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(n);
        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(e);

        // Initialise the public key structure.
        if ((this->_key = ::EVP_PKEY_new()) == nullptr)
          throw Exception(::ERR_error_string(ERR_get_error(), nullptr));

        ::RSA* rsa;

        // Create the RSA structure.
        if ((rsa = ::RSA_new()) == nullptr)
          throw Exception(::ERR_error_string(ERR_get_error(), nullptr));

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);

        // Assign the big numbers relevant to the public key.
        rsa->n = n;
        rsa->e = e;

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(n);
        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(e);

        // Set the rsa structure into the public key.
        if (::EVP_PKEY_assign_RSA(this->_key, rsa) <= 0)
          throw Exception(::ERR_error_string(ERR_get_error(), nullptr));

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);
      }

      void
      PublicKey::_prepare()
      {
        ELLE_DEBUG_FUNCTION("");

        // Prepare the encrypt context.
        if ((this->_context_encrypt =
             ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
          throw Exception(::ERR_error_string(ERR_get_error(), nullptr));

        if (::EVP_PKEY_encrypt_init(this->_context_encrypt) <= 0)
          throw Exception(::ERR_error_string(ERR_get_error(), nullptr));

        if (::EVP_PKEY_CTX_ctrl(this->_context_encrypt,
                                EVP_PKEY_RSA,
                                -1,
                                EVP_PKEY_CTRL_RSA_PADDING,
                                RSA_PKCS1_OAEP_PADDING,
                                nullptr) <= 0)
          throw Exception(::ERR_error_string(ERR_get_error(), nullptr));

        // Prepare the verify context.
        if ((this->_context_verify =
             ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
          throw Exception(::ERR_error_string(ERR_get_error(), nullptr));

        if (::EVP_PKEY_verify_init(this->_context_verify) <= 0)
          throw Exception(::ERR_error_string(ERR_get_error(), nullptr));

        if (::EVP_PKEY_CTX_ctrl(this->_context_verify,
                                EVP_PKEY_RSA,
                                -1,
                                EVP_PKEY_CTRL_RSA_PADDING,
                                RSA_PKCS1_PADDING,
                                nullptr) <= 0)
          throw Exception(::ERR_error_string(ERR_get_error(), nullptr));

        // Prepare the decrypt context.
        if ((this->_context_decrypt =
             ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
          throw Exception(::ERR_error_string(ERR_get_error(), nullptr));

        if (::EVP_PKEY_verify_recover_init(this->_context_decrypt) <= 0)
          throw Exception(::ERR_error_string(ERR_get_error(), nullptr));

        if (::EVP_PKEY_CTX_ctrl(this->_context_decrypt,
                                EVP_PKEY_RSA,
                                -1,
                                EVP_PKEY_CTRL_RSA_PADDING,
                                RSA_PKCS1_PADDING,
                                nullptr) <= 0)
          throw Exception(::ERR_error_string(ERR_get_error(), nullptr));
      }

      /*----------.
      | Operators |
      `----------*/

      elle::Boolean
      PublicKey::operator ==(PublicKey const& other) const
      {
        if (this == &other)
          return (true);

        ELLE_ASSERT(this->_key != nullptr);
        ELLE_ASSERT(other._key != nullptr);

        // Compare the internal numbers.
        if ((::BN_cmp(this->_key->pkey.rsa->n,
                      other._key->pkey.rsa->n) != 0) ||
            (::BN_cmp(this->_key->pkey.rsa->e,
                      other._key->pkey.rsa->e) != 0))
          return (false);

        return (true);
      }

      elle::Boolean
      PublicKey::operator <(PublicKey const& other) const
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

        elle::unreachable();
      }

      /*-----------.
      | Public Key |
      `-----------*/

      elle::Boolean
      PublicKey::operator ==(publickey::Interface const& other) const
      {
        if (this == &other)
          return (true);

        if (this->cryptosystem() != other.cryptosystem())
          return (false);

        return (*this == static_cast<PublicKey const&>(other));
      }

      elle::Boolean
      PublicKey::operator <(publickey::Interface const& other) const
      {
        if (this == &other)
          return (true);

        if (this->cryptosystem() != other.cryptosystem())
          return (this->cryptosystem() < other.cryptosystem());

        return (*this < static_cast<PublicKey const&>(other));
      }

      publickey::Interface*
      PublicKey::clone() const
      {
        return (new PublicKey(*this));
      }

      elle::Natural32
      PublicKey::size() const
      {
        return (static_cast<elle::Natural32>(::EVP_PKEY_size(this->_key)));
      }

      elle::Natural32
      PublicKey::length() const
      {
        return (static_cast<elle::Natural32>(::EVP_PKEY_bits(this->_key)));
      }

      Cryptosystem
      PublicKey::cryptosystem() const
      {
        return (Cryptosystem::rsa);
      }

      Code
      PublicKey::encrypt(Plain const& plain) const
      {
        ELLE_TRACE_METHOD(plain);

        return (evp::asymmetric::encrypt(plain,
                                         this->_context_encrypt,
                                         ::EVP_PKEY_encrypt));
      }

      elle::Boolean
      PublicKey::verify(Signature const& signature,
                        Plain const& plain) const
      {
        ELLE_TRACE_METHOD(signature, plain);

        return (evp::asymmetric::verify(signature,
                                        plain,
                                        this->_context_verify,
                                        ::EVP_PKEY_verify));
      }

      Clear
      PublicKey::decrypt(Code const& code) const
      {
        ELLE_TRACE_METHOD(code);

        return (evp::asymmetric::decrypt(code,
                                         this->_context_decrypt,
                                         ::EVP_PKEY_verify_recover));
      }

      /*----------.
      | Printable |
      `----------*/

      void
      PublicKey::print(std::ostream& stream) const
      {
        ELLE_ASSERT(this->_key != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->n != nullptr);
        ELLE_ASSERT(this->_key->pkey.rsa->e != nullptr);

        stream << "("
               << *this->_key->pkey.rsa->n
               << ", "
               << *this->_key->pkey.rsa->e
               << ")";
      }
    }
  }
}
