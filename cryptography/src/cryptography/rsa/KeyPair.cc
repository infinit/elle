#include <cryptography/Error.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/deleter.hh>
#include <cryptography/finally.hh>
#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/Seed.hh>
#include <cryptography/types.hh>

#include <elle/attribute.hh>
#include <elle/assert.hh>
#include <elle/log.hh>

#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
# include <dopenssl/rsa.hh>
#endif

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /*-------------.
      | Construction |
      `-------------*/

      KeyPair::KeyPair(PublicKey const& K, PrivateKey const& k)
        : _public_key(new PublicKey(K))
        , _private_key(new PrivateKey(k))
      {}

      KeyPair::KeyPair(std::shared_ptr<PublicKey> K,
                       std::shared_ptr<PrivateKey> k)
        : _public_key(std::move(K))
        , _private_key(std::move(k))
      {}

      KeyPair::KeyPair(PublicKey&& K,
                       PrivateKey&& k):
        _public_key(new PublicKey(std::move(K))),
        _private_key(new PrivateKey(std::move(k)))
      {}

      KeyPair::KeyPair(KeyPair const& other):
        _public_key(new PublicKey(*other._public_key)),
        _private_key(new PrivateKey(*other._private_key))
      {}

      KeyPair::KeyPair(KeyPair&& other):
        _public_key(std::move(other._public_key)),
        _private_key(std::move(other._private_key))
      {}

      KeyPair::~KeyPair()
      {}

      /*--------.
      | Methods |
      `--------*/

      PublicKey const&
      KeyPair::K() const
      {
        ELLE_ASSERT_NEQ(this->_public_key, nullptr);

        return (*this->_public_key);
      }

      PrivateKey const&
      KeyPair::k() const
      {
        ELLE_ASSERT_NEQ(this->_private_key, nullptr);

        return (*this->_private_key);
      }

      uint32_t
      KeyPair::size() const
      {
        ELLE_ASSERT_NEQ(this->_public_key, nullptr);
        ELLE_ASSERT_NEQ(this->_private_key, nullptr);
        ELLE_ASSERT_EQ(this->_public_key->size(), this->_private_key->size());

        return (this->_public_key->size());
      }

      uint32_t
      KeyPair::length() const
      {
        ELLE_ASSERT_NEQ(this->_public_key, nullptr);
        ELLE_ASSERT_NEQ(this->_private_key, nullptr);
        ELLE_ASSERT_EQ(this->_public_key->length(),
                       this->_private_key->length());

        return (this->_public_key->length());
      }

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
      /*---------.
      | Rotation |
      `---------*/

      KeyPair::KeyPair(Seed const& seed)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Deduce the RSA key from the given seed.
        ::RSA* rsa = nullptr;

        if ((rsa = ::dRSA_deduce_privatekey(
               seed.length(),
               static_cast<unsigned char const*>(seed.buffer().contents()),
               seed.buffer().size())) == nullptr)
          throw Error(
            elle::sprintf("unable to deduce the RSA key from the given "
                          "seed: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);

        // Instanciate both a RSA public and private key based on the RSA
        // structure.
        this->_private_key.reset(new PrivateKey(rsa));
        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);
        this->_public_key.reset(new PublicKey(*this->_private_key));
      }
#endif

      /*----------.
      | Operators |
      `----------*/

      bool
      KeyPair::operator ==(KeyPair const& other) const
      {
        if (this == &other)
          return (true);

        ELLE_ASSERT_NEQ(this->_public_key, nullptr);
        ELLE_ASSERT_NEQ(this->_private_key, nullptr);

        // The public component is enough to uniquely identify a key pair.
        return (*this->_public_key == *other._public_key);
      }

      /*--------------.
      | Serialization |
      `--------------*/

      KeyPair::KeyPair(elle::serialization::SerializerIn& serializer):
        _public_key(),
        _private_key()
      {
        this->serialize(serializer);
      }

      void
      KeyPair::serialize(elle::serialization::Serializer& serializer)
      {
        serializer.serialize("public key", this->_public_key);
        if (this->_public_key == nullptr)
          throw Error(
            elle::sprintf("unable to deserialize the 'public key'"));

        serializer.serialize("private key", this->_private_key);
        if (this->_private_key == nullptr)
          throw Error(
            elle::sprintf("unable to deserialize the 'private key'"));
      }

      /*----------.
      | Printable |
      `----------*/

      void
      KeyPair::print(std::ostream& stream) const
      {
        ELLE_ASSERT_NEQ(this->_public_key, nullptr);
        ELLE_ASSERT_NEQ(this->_private_key, nullptr);

        stream << "(" << *this->_public_key << ", "
               << *this->_private_key << ")";
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
      namespace keypair
      {
        /*----------.
        | Functions |
        `----------*/

        KeyPair
        generate(uint32_t const length)
        {
          if ((length % 8) != 0)
            throw Error(
              elle::sprintf("the keypair length must be a multiple of 8"));

          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Retrieve the key pair generation context.
          types::EVP_PKEY_CTX context(
            ::EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr));

          if (context == nullptr)
            throw Error(
              elle::sprintf("unable to allocate a keypair generation "
                            "context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Initialise the context for key generation.
          if (::EVP_PKEY_keygen_init(context.get()) <= 0)
            throw Error(
              elle::sprintf("unable to initialize the keypair generation "
                            "context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Set the key length in the keypair generation context.
          if (::EVP_PKEY_CTX_set_rsa_keygen_bits(context.get(), length) <= 0)
            throw Error(
              elle::sprintf("unable to set the length of the keypair to "
                            "be generated: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ::EVP_PKEY* key = nullptr;

          // Generate the EVP key.
          if (::EVP_PKEY_keygen(context.get(), &key) <= 0)
            throw Error(
              elle::sprintf("unable to generate a keypair: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

          ELLE_ASSERT_NEQ(key, nullptr);

          // Instanciate both a RSA public and private key based on the
          // EVP_PKEY.
          PrivateKey k(key);
          PublicKey K(k);

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(key);

          return (KeyPair(std::move(K), std::move(k)));
        }

        namespace der
        {
          /*----------.
          | Functions |
          `----------*/

          elle::Buffer
          encode(KeyPair const& keypair)
          {
            return (privatekey::der::encode(keypair.k()));
          }

          KeyPair
          decode(elle::ConstWeakBuffer const& buffer)
          {
            PrivateKey k = privatekey::der::decode(buffer);
            PublicKey K(k);

            return (KeyPair(K, k));
          }
        }
      }
    }
  }
}
