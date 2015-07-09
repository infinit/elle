#include <cryptography/dsa/PrivateKey.hh>
#include <cryptography/dsa/KeyPair.hh>
#include <cryptography/dsa/serialization.hh>
#include <cryptography/dsa/der.hh>
#include <cryptography/context.hh>
#include <cryptography/Exception.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/bn.hh>
#include <cryptography/raw.hh>
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
      namespace privatekey
      {
        /*--------------.
        | Serialization |
        `--------------*/

        struct Serialization:
          public dsa::serialization::DSA
        {
          static
          elle::Buffer
          encode(::DSA* dsa)
          {
            return der::encode_private(dsa);
          }

          static
          ::DSA*
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

        if (::EVP_PKEY_type(this->_key->type) != EVP_PKEY_DSA)
          throw Exception(
            elle::sprintf("the EVP_PKEY key is not of type DSA: %s",
                          ::EVP_PKEY_type(this->_key->type)));

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

      elle::Buffer
      PrivateKey::sign(elle::ConstWeakBuffer const& plain) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("plain: %x", plain);

        elle::IOStream _plain(plain.istreambuf());

        return (this->sign(_plain));
      }

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

        // Allocate the EVP key to receive the deserialized's DSA structure.
        this->_key.reset(::EVP_PKEY_new());

        // Set the EVP key as being of type DSA.
        if (::EVP_PKEY_set_type(this->_key.get(), EVP_PKEY_DSA) <= 0)
          throw Exception(
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
          this->_key->pkey.dsa);

        serializer.serialize("digest algorithm", this->_digest_algorithm);
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

        stream << "("
               << *this->_key->pkey.dsa->pub_key
               << ", "
               << *this->_key->pkey.dsa->priv_key
               << ")";

        stream << "["
               << this->_digest_algorithm
               << "]";
      }
    }
  }
}
