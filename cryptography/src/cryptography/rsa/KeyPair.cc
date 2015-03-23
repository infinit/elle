#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/rsa/Seed.hh>
#include <cryptography/Exception.hh>
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

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
# include <dopenssl/rsa.hh>
#endif

ELLE_LOG_COMPONENT("infinit.cryptography.rsa.keypair");

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /*-------------.
      | Construction |
      `-------------*/

      KeyPair::KeyPair()
      {
      }

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

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
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
          throw Exception(
            elle::sprintf("unable to deduce the RSA key from the given "
                          "seed: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);

        // Instanciate both a RSA public and private key based on the RSA
        // structure.
        //
        // Note that some internal numbers need to be duplicated for the
        // instanciation of the public key while the private key is
        // constructed by transferring the ownership of the RSA.
        this->_K.reset(new PublicKey(::BN_dup(rsa->n),
                                     ::BN_dup(rsa->e)));
        this->_k.reset(new PrivateKey(rsa));

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);
      }
#endif

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

      ELLE_SERIALIZE_CONSTRUCT_DEFINE(KeyPair)
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

      elle::Natural32
      KeyPair::size() const
      {
        ELLE_ASSERT_NEQ(this->_K, nullptr);
        ELLE_ASSERT_NEQ(this->_k, nullptr);
        ELLE_ASSERT_EQ(this->_K->size(), this->_k->size());

        return (this->_K->size());
      }

      elle::Natural32
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

      elle::Boolean
      KeyPair::operator ==(KeyPair const& other) const
      {
        if (this == &other)
          return (true);

        ELLE_ASSERT_NEQ(this->_K, nullptr);
        ELLE_ASSERT_NEQ(this->_k, nullptr);

        return ((*this->_K == *other._K) && (*this->_k == *other._k));
      }

      elle::Boolean
      KeyPair::operator <(KeyPair const& other) const
      {
        if (this == &other)
          return (false);

        ELLE_ASSERT_NEQ(this->_K, nullptr);
        ELLE_ASSERT_NEQ(this->_k, nullptr);

        if (*this->_K < *other._K)
          return (true);
        else if (*this->_K > *other._K)
          return (false);

        if (*this->_k < *other._k)
          return (true);
        else
          return (false);

        elle::unreachable();
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
      KeyPair::serialize(elle::serialization::Serializer& s)
      {
        s.serialize("public_key", this->_K);
        s.serialize("private_key", this->_k);
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

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace keypair
      {
        /*--------.
        | Classes |
        `--------*/

        // The key pair initializer.
        class Initializer
        {
        public:
          Initializer()
          {
            // Create the context for the RSA algorithm.
            this->_context.reset(
              ::EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr));

            if (this->_context == nullptr)
              throw Exception(
                elle::sprintf("unable to allocate a keypair generation "
                              "context: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            // Initialise the context for key generation.
            if (::EVP_PKEY_keygen_init(this->_context.get()) <= 0)
              throw Exception(
                elle::sprintf("unable to initialize the keypair generation "
                              "context: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));
          }

        private:
          ELLE_ATTRIBUTE_R(types::EVP_PKEY_CTX, context);
        };

        /*-----------------.
        | Static Functions |
        `-----------------*/

        /// Construct the key pair generation context.
        static
        ::EVP_PKEY_CTX*
        _context()
        {
          static Initializer initialized;

          ELLE_ASSERT_NEQ(initialized.context(), nullptr);

          return (initialized.context().get());
        }

        /*----------.
        | Functions |
        `----------*/

        KeyPair
        generate(elle::Natural32 const length)
        {
          ELLE_TRACE_FUNCTION(length);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Retrieve the key pair generation context.
          static ::EVP_PKEY_CTX* context = _context();

          // Set the key length in the keypair generation context.
          if (::EVP_PKEY_CTX_set_rsa_keygen_bits(context, length) <= 0)
            throw Exception(
              elle::sprintf("unable to set the length of the keypair to "
                            "be generated: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ::EVP_PKEY* key = nullptr;

          // Generate the EVP key.
          if (::EVP_PKEY_keygen(context, &key) <= 0)
            throw Exception(
              elle::sprintf("unable to generate a keypair: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

          ELLE_ASSERT_NEQ(key, nullptr);

          // Instanciate both a RSA public and private key based on the
          // EVP_PKEY.
          //
          // Note that some internal numbers need to be duplicated for the
          // instanciation of the public key while the private key is
          // constructed by transferring the ownership of the EVP_PKEY.
          PublicKey K(::BN_dup(key->pkey.rsa->n),
                      ::BN_dup(key->pkey.rsa->e));
          PrivateKey k(key);

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(key);

          return (KeyPair(std::move(K), std::move(k)));
        }
      }
    }
  }
}
