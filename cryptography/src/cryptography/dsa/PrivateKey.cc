#include <cryptography/dsa/PrivateKey.hh>
#include <cryptography/dsa/KeyPair.hh>
#include <cryptography/dsa/context.hh>
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
#include <openssl/dsa.h>
#include <openssl/err.h>

ELLE_LOG_COMPONENT("infinit.cryptography.dsa.PrivateKey");

namespace infinit
{
  namespace cryptography
  {
    namespace dsa
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
                             Oneway const digest_algorithm):
        _key(key),
        _digest_algorithm(digest_algorithm)
      {
        ELLE_ASSERT_NEQ(key, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.dsa->pub_key, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.dsa->priv_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Prepare the cryptographic contexts.
        this->_prepare();

        this->_check();
      }

      PrivateKey::PrivateKey(::DSA* dsa,
                             Oneway const digest_algorithm):
        _digest_algorithm(digest_algorithm)
      {
        ELLE_ASSERT_NEQ(dsa, nullptr);
        ELLE_ASSERT_NEQ(dsa->pub_key, nullptr);
        ELLE_ASSERT_NEQ(dsa->priv_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Construct the private key based on the given DSA structure.
        this->_construct(dsa);

        // Prepare the cryptographic contexts.
        this->_prepare();

        this->_check();
      }

      PrivateKey::PrivateKey(PrivateKey const& other):
        _digest_algorithm(other._digest_algorithm)
      {
        ELLE_ASSERT_NEQ(other._key->pkey.dsa->pub_key, nullptr);
        ELLE_ASSERT_NEQ(other._key->pkey.dsa->priv_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Duplicate the DSA structure.
        DSA* _dsa = low::DSA_dup(other._key->pkey.dsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_DSA(_dsa);

        this->_construct(_dsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(_dsa);

        // Prepare the cryptographic contexts.
        this->_prepare();

        this->_check();
      }

      PrivateKey::PrivateKey(PrivateKey&& other):
        _key(std::move(other._key)),
        _digest_algorithm(std::move(other._digest_algorithm))
      {
        this->_context.sign = std::move(other._context.sign);

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
      PrivateKey::_construct(::DSA* dsa)
      {
        ELLE_DEBUG_FUNCTION(dsa);

        ELLE_ASSERT_NEQ(dsa, nullptr);

        // Initialise the private key structure.
        ELLE_ASSERT_EQ(this->_key, nullptr);
        this->_key.reset(::EVP_PKEY_new());

        if (this->_key == nullptr)
          throw Exception(
            elle::sprintf("unable to allocate the EVP_PKEY structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Set the dsa structure into the private key.
        if (::EVP_PKEY_assign_DSA(this->_key.get(), dsa) <= 0)
          throw Exception(
            elle::sprintf("unable to assign the DSA key to the EVP_PKEY "
                          "structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
      }

      void
      PrivateKey::_prepare()
      {
        ELLE_DEBUG_FUNCTION("");

        ELLE_ASSERT_NEQ(this->_key, nullptr);

        // Prepare the sign context.
        ELLE_ASSERT_EQ(this->_context.sign, nullptr);
        this->_context.sign.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_sign_init));
      }

      void
      PrivateKey::_check() const
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa->pub_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa->priv_key, nullptr);
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

      Signature
      PrivateKey::sign(Plain const& plain) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("plain: %x", plain);

        Digest digest = hash(plain, this->_digest_algorithm);

        return (this->sign(digest));
      }

      // XXX derive

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
        std::unique_ptr<DSA, void (*) (::DSA*)> dsa(::DSA_new(), &::DSA_free);
        if (!dsa)
          throw elle::Error(
            elle::sprintf("unable to initialize DSA: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
        if (::EVP_PKEY_assign_DSA(this->_key.get(), dsa.get()) <= 0)
          throw elle::Error(
            elle::sprintf("unable to assign the DSA: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
        dsa.release();
        this->serialize(serializer);
        this->_prepare();
      }

      void
      PrivateKey::serialize(elle::serialization::Serializer& serializer)
      {
        // XXX
      }

      /*----------.
      | Printable |
      `----------*/

      void
      PrivateKey::print(std::ostream& stream) const
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa->pub_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa->priv_key, nullptr);

        /* XXX
        stream << "("
               << *this->_key->pkey.dsa->n
               << ", "
               << *this->_key->pkey.dsa->e
               << ", "
               << *this->_key->pkey.dsa->d
               << ")";

        stream << "["
               << this->_digest_algorithm
               << "]";
        */
      }
    }
  }
}
