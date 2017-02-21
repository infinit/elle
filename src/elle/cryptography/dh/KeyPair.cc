#include <elle/cryptography/dh/KeyPair.hh>
#include <elle/cryptography/Error.hh>
#include <elle/cryptography/cryptography.hh>
#include <elle/cryptography/finally.hh>
#include <elle/cryptography/deleter.hh>
#include <elle/cryptography/types.hh>

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
    namespace dh
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
    namespace dh
    {
      namespace keypair
      {
        /*----------.
        | Functions |
        `----------*/

        KeyPair
        generate()
        {
          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Integrate the parameters.
          ::EVP_PKEY* parameters = nullptr;

          if ((parameters = ::EVP_PKEY_new()) == nullptr)
            throw Error(
              elle::sprintf("unable to allocate a parameters object: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(parameters);

          if (::EVP_PKEY_assign_DH(parameters, ::DH_get_2048_256()) <= 0)
              throw Error(
                elle::sprintf("unable to set the parameters: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

          // Prepare the key generation context.
          ::EVP_PKEY_CTX* context;

          if ((context = ::EVP_PKEY_CTX_new(parameters, nullptr)) == nullptr)
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

          // Generate the Diffie Hellman key pair.
          ::EVP_PKEY* key = nullptr;

          if (::EVP_PKEY_keygen(context, &key) <= 0)
            throw Error(
              elle::sprintf("unable to generate a keypair: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

          ELLE_ASSERT_NEQ(key, nullptr);

          // Instanciate both a DH public and private key based on the
          // EVP_PKEY.
          PrivateKey k(key);
          PublicKey K(k);

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(key);

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(context);
          ::EVP_PKEY_CTX_free(context);

          ELLE_CRYPTOGRAPHY_FINALLY_ABORT(parameters);
          ::EVP_PKEY_free(parameters);

          return (KeyPair(std::move(K), std::move(k)));
        }
      }
    }
  }
}
