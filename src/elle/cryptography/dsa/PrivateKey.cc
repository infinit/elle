#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/dsa.h>
#include <openssl/err.h>

#include <elle/log.hh>

#include <elle/cryptography/Error.hh>
#include <elle/cryptography/bn.hh>
#include <elle/cryptography/cryptography.hh>
#include <elle/cryptography/dsa/KeyPair.hh>
#include <elle/cryptography/dsa/PrivateKey.hh>
#include <elle/cryptography/dsa/der.hh>
#include <elle/cryptography/dsa/low.hh>
#include <elle/cryptography/dsa/serialization.hh>
#include <elle/cryptography/finally.hh>
#include <elle/cryptography/hash.hh>
#include <elle/cryptography/raw.hh>

namespace elle
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
          throw Error(
            elle::sprintf("the EVP_PKEY key is not of type DSA: %s",
                          ::EVP_PKEY_type(this->_key->type)));

        // Construct the private key based on the given DSA structure.
        this->_construct(dsa);

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

        ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_DSA(_dsa);

        this->_construct(_dsa);

        ELLE_CRYPTOGRAPHY_FINALLY_ABORT(_dsa);

        this->_check();
      }

      PrivateKey::PrivateKey(PrivateKey&& other):
        _key(std::move(other._key)),
        _digest_algorithm(std::move(other._digest_algorithm))
      {
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
        ELLE_ASSERT_NEQ(dsa, nullptr);

        // Initialise the private key structure.
        ELLE_ASSERT_EQ(this->_key, nullptr);
        this->_key.reset(::EVP_PKEY_new());

        if (this->_key == nullptr)
          throw Error(
            elle::sprintf("unable to allocate the EVP_PKEY structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Set the dsa structure into the private key.
        if (::EVP_PKEY_assign_DSA(this->_key.get(), dsa) <= 0)
          throw Error(
            elle::sprintf("unable to assign the DSA key to the EVP_PKEY "
                          "structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
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
        elle::IOStream _plain(plain.istreambuf());

        return (this->sign(_plain));
      }

      elle::Buffer
      PrivateKey::sign(std::istream& plain) const
      {
        return (raw::asymmetric::sign(
                  this->_key.get(),
                  oneway::resolve(this->_digest_algorithm),
                  plain));
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
          this->_key->pkey.dsa);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa, nullptr);

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
