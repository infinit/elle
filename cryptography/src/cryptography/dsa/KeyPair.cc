#include <cryptography/dsa/KeyPair.hh>
#include <cryptography/Error.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/deleter.hh>
#include <cryptography/types.hh>

#include <elle/attribute.hh>
#include <elle/assert.hh>
#include <elle/log.hh>

#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/evp.h>

namespace elle
{
  namespace cryptography
  {
    namespace dsa
    {
      /*-------------.
      | Construction |
      `-------------*/

      KeyPair::KeyPair(PublicKey const& K,
                       PrivateKey const& k):
        _K(new PublicKey(K)),
        _k(new PrivateKey(k))
      {
      }

      KeyPair::KeyPair(PublicKey&& K,
                       PrivateKey&& k):
        _K(new PublicKey(std::move(K))),
        _k(new PrivateKey(std::move(k)))
      {
      }

      KeyPair::KeyPair(KeyPair const& other):
        _K(new PublicKey(*other._K)),
        _k(new PrivateKey(*other._k))
      {
      }

      KeyPair::KeyPair(KeyPair&& other):
        _K(std::move(other._K)),
        _k(std::move(other._k))
      {
      }

      /*--------.
      | Methods |
      `--------*/

      PublicKey const&
      KeyPair::K() const
      {
        ELLE_ASSERT_NEQ(this->_K, nullptr);

        return (*this->_K);
      }

      PrivateKey const&
      KeyPair::k() const
      {
        ELLE_ASSERT_NEQ(this->_k, nullptr);

        return (*this->_k);
      }

      uint32_t
      KeyPair::size() const
      {
        ELLE_ASSERT_NEQ(this->_K, nullptr);
        ELLE_ASSERT_NEQ(this->_k, nullptr);
        ELLE_ASSERT_EQ(this->_K->size(), this->_k->size());

        return (this->_K->size());
      }

      uint32_t
      KeyPair::length() const
      {
        ELLE_ASSERT_NEQ(this->_K, nullptr);
        ELLE_ASSERT_NEQ(this->_k, nullptr);
        ELLE_ASSERT_EQ(this->_K->length(), this->_k->length());

        return (this->_K->length());
      }

      /*----------.
      | Operators |
      `----------*/

      bool
      KeyPair::operator ==(KeyPair const& other) const
      {
        if (this == &other)
          return (true);

        ELLE_ASSERT_NEQ(this->_K, nullptr);
        ELLE_ASSERT_NEQ(this->_k, nullptr);

        // The public component is enough to uniquely identify a key pair.
        return (*this->_K == *other._K);
      }

      /*--------------.
      | Serialization |
      `--------------*/

      KeyPair::KeyPair(elle::serialization::SerializerIn& serializer):
        _K(),
        _k()
      {
        this->serialize(serializer);
      }

      void
      KeyPair::serialize(elle::serialization::Serializer& serializer)
      {
        serializer.serialize("public key", this->_K);
        if (this->_K == nullptr)
          throw Error(
            elle::sprintf("unable to deserialize the 'public key'"));

        serializer.serialize("private key", this->_k);
        if (this->_k == nullptr)
          throw Error(
            elle::sprintf("unable to deserialize the 'private key'"));
      }

      /*----------.
      | Printable |
      `----------*/

      void
      KeyPair::print(std::ostream& stream) const
      {
        ELLE_ASSERT_NEQ(this->_K, nullptr);
        ELLE_ASSERT_NEQ(this->_k, nullptr);

        stream << "(" << *this->_K << ", " << *this->_k << ")";
      }
    }
  }
}

//
// ---------- Generator -------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace dsa
    {
      namespace keypair
      {
        /*----------.
        | Functions |
        `----------*/

        KeyPair
        generate(uint32_t const length,
                 Oneway const digest_algorithm)
        {
          // Make sure the cryptographic system is set up.
          cryptography::require();

          ::EVP_PKEY* parameters = nullptr;

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(parameters);

          // Generate the parameters for the DSA key.
          {
            ::EVP_PKEY_CTX* context;

            if ((context =
                 ::EVP_PKEY_CTX_new_id(EVP_PKEY_DSA, nullptr)) == nullptr)
              throw Error(
                elle::sprintf("unable to allocate a parameters generation "
                              "context: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY_CONTEXT(context);

            if (::EVP_PKEY_paramgen_init(context) <= 0)
              throw Error(
                elle::sprintf("unable to initialize the parameters generation "
                              "context: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            if (::EVP_PKEY_CTX_set_dsa_paramgen_bits(context, length) <= 0)
              throw Error(
                elle::sprintf("unable to set the parameters generation "
                              "context's key length: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            if (::EVP_PKEY_paramgen(context, &parameters) <= 0)
              throw Error(
                elle::sprintf("unable to generate the parameters: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            ELLE_CRYPTOGRAPHY_FINALLY_ABORT(context);
            ::EVP_PKEY_CTX_free(context);
          }

          ::EVP_PKEY* key = nullptr;

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

          // Generate the DSA key pair.
          {
            ::EVP_PKEY_CTX* context;

            if ((context =
                 ::EVP_PKEY_CTX_new(parameters, nullptr)) == nullptr)
              throw Error(
                elle::sprintf("unable to allocate a keypair generation "
                              "context: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY_CONTEXT(context);

            if (::EVP_PKEY_keygen_init(context) <= 0)
              throw Error(
                elle::sprintf("unable to initialize the keypair generation "
                              "context: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            if (::EVP_PKEY_keygen(context, &key) <= 0)
              throw Error(
                elle::sprintf("unable to generate a keypair: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            ELLE_CRYPTOGRAPHY_FINALLY_ABORT(context);
            ::EVP_PKEY_CTX_free(context);
          }

          ELLE_ASSERT_NEQ(key, nullptr);

          // Instanciate both a DSA public and private key based on the
          // EVP_PKEY.
          PrivateKey k(key,
                       digest_algorithm);
          PublicKey K(k);

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(key);

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(parameters);
          ::EVP_PKEY_free(parameters);

          return (KeyPair(std::move(K), std::move(k)));
        }
      }
    }
  }
}
