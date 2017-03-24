#include <openssl/crypto.h>
#include <openssl/dsa.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#include <elle/Error.hh>
#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/printf.hh>

#include <elle/cryptography/dsa/PublicKey.hh>
#include <elle/cryptography/dsa/PrivateKey.hh>
#include <elle/cryptography/dsa/KeyPair.hh>
#include <elle/cryptography/dsa/der.hh>
#include <elle/cryptography/dsa/serialization.hh>
#include <elle/cryptography/dsa/low.hh>
#include <elle/cryptography/Error.hh>
#include <elle/cryptography/cryptography.hh>
#include <elle/cryptography/bn.hh>
#include <elle/cryptography/finally.hh>
#include <elle/cryptography/raw.hh>
#include <elle/cryptography/hash.hh>

#include <elle/serialization/binary.hh>

//
// ---------- Class -----------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace dsa
    {
      namespace publickey
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
            return der::encode_public(dsa);
          }

          static
          ::DSA*
          decode(elle::ConstWeakBuffer const& buffer)
          {
            return der::decode_public(buffer);
          }
        };
      }

      /*-------------.
      | Construction |
      `-------------*/

      PublicKey::PublicKey(PrivateKey const& k)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Set the key parameters.
        this->_digest_algorithm = k.digest_algorithm();

        // Extract the public key only.
        DSA* _dsa = low::DSA_priv2pub(k.key().get()->pkey.dsa);

        ELLE_ASSERT_NEQ(_dsa->pub_key, nullptr);
        ELLE_ASSERT_EQ(_dsa->priv_key, nullptr);

        ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_DSA(_dsa);

        // Construct the public key based on the given DSA structure whose
        // ownership is retained.
        this->_construct(_dsa);

        ELLE_CRYPTOGRAPHY_FINALLY_ABORT(_dsa);

        this->_check();
      }

      PublicKey::PublicKey(::EVP_PKEY* key,
                           Oneway const digest_algorithm):
        _key(key),
        _digest_algorithm(digest_algorithm)
      {
        ELLE_ASSERT_NEQ(key, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.dsa->pub_key, nullptr);
        ELLE_ASSERT_EQ(key->pkey.dsa->priv_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        if (::EVP_PKEY_type(this->_key->type) != EVP_PKEY_DSA)
          throw Error(
            elle::sprintf("the EVP_PKEY key is not of type DSA: %s",
                          ::EVP_PKEY_type(this->_key->type)));

        this->_check();
      }

      PublicKey::PublicKey(::DSA* dsa,
                           Oneway const digest_algorithm):
        _digest_algorithm(digest_algorithm)
      {
        ELLE_ASSERT_NEQ(dsa, nullptr);
        ELLE_ASSERT_NEQ(dsa->pub_key, nullptr);
        ELLE_ASSERT_EQ(dsa->priv_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Construct the public key based on the given DSA structure.
        this->_construct(dsa);

        this->_check();
      }

      PublicKey::PublicKey(PublicKey const& other):
        _digest_algorithm(other._digest_algorithm)
      {
        ELLE_ASSERT_NEQ(other._key->pkey.dsa->pub_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Duplicate the DSA structure.
        DSA* _dsa = low::DSA_dup(other._key->pkey.dsa);

        ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_DSA(_dsa);

        this->_construct(_dsa);

        ELLE_CRYPTOGRAPHY_FINALLY_ABORT(_dsa);

        this->_check();
      }

      PublicKey::PublicKey(PublicKey&& other):
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
      PublicKey::_construct(::DSA* dsa)
      {
        // Initialise the public key structure.
        ELLE_ASSERT_EQ(this->_key, nullptr);
        this->_key.reset(::EVP_PKEY_new());

        if (this->_key == nullptr)
          throw Error(
            elle::sprintf("unable to allocate the EVP_PKEY structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Set the dsa structure into the public key.
        if (::EVP_PKEY_assign_DSA(this->_key.get(), dsa) <= 0)
          throw Error(
            elle::sprintf("unable to assign the DSA key to the EVP_PKEY "
                          "structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
      }

      void
      PublicKey::_check() const
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa->pub_key, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.dsa->priv_key, nullptr);
      }

      bool
      PublicKey::verify(elle::ConstWeakBuffer const& signature,
                        elle::ConstWeakBuffer const& plain) const
      {
        elle::IOStream _plain(plain.istreambuf());
        return (this->verify(signature, _plain));
      }

      bool
      PublicKey::verify(elle::ConstWeakBuffer const& signature,
                        std::istream& plain) const
      {
        return (raw::asymmetric::verify(
                  this->_key.get(),
                  oneway::resolve(this->_digest_algorithm),
                  signature,
                  plain));
      }

      uint32_t
      PublicKey::size() const
      {
        return (static_cast<uint32_t>(
                  ::EVP_PKEY_size(this->_key.get())));
      }

      uint32_t
      PublicKey::length() const
      {
        return (static_cast<uint32_t>(
                  ::EVP_PKEY_bits(this->_key.get())));
      }

      /*----------.
      | Operators |
      `----------*/

      bool
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
      PublicKey::serialize(elle::serialization::Serializer& serializer)
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);

        cryptography::serialize<publickey::Serialization>(
          serializer,
          this->_key->pkey.dsa);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa, nullptr);

        serializer.serialize("digest algorithm", this->_digest_algorithm);
      }

      /*----------.
      | Printable |
      `----------*/

      void
      PublicKey::print(std::ostream& stream) const
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa->pub_key, nullptr);

        stream << "("
               << *this->_key->pkey.dsa->pub_key
               << ")";

        stream << "["
               << this->_digest_algorithm
               << "]";
      }
    }
  }
}

namespace std
{
  size_t
  hash<elle::cryptography::dsa::PublicKey>::operator ()(
    elle::cryptography::dsa::PublicKey const& value) const
  {
    std::stringstream stream;
    {
      elle::serialization::binary::SerializerOut output(stream);
      output.serialize("value", value);
    }

    return std::hash<std::string>()(stream.str());
  }
}
