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

ELLE_LOG_COMPONENT("infinit.cryptography.rsa.KeyPair");

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /*-------------.
      | Construction |
      `-------------*/

      KeyPair::KeyPair(PublicKey const& K,
                       PrivateKey const& k):
        _K(new PublicKey(K)),
        _k(new PrivateKey(k))
      {}

      KeyPair::KeyPair(PublicKey&& K,
                       PrivateKey&& k):
        _K(new PublicKey(std::move(K))),
        _k(new PrivateKey(std::move(k)))
      {}

      KeyPair::KeyPair(KeyPair const& other):
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        elle::serialize::DynamicFormat<KeyPair>(other),
#endif
        _K(new PublicKey(*other._K)),
        _k(new PrivateKey(*other._k))
      {}

      KeyPair::KeyPair(KeyPair&& other):
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        elle::serialize::DynamicFormat<KeyPair>(other),
#endif
        _K(std::move(other._K)),
        _k(std::move(other._k))
      {}

      KeyPair::~KeyPair()
      {}

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

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
      /*---------.
      | Rotation |
      `---------*/

      KeyPair::KeyPair(Seed const& seed,
                       Padding const encryption_padding,
                       Padding const signature_padding,
                       Oneway const digest_algorithm,
                       Cipher const envelope_cipher,
                       Mode const envelope_mode)
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
        this->_k.reset(new PrivateKey(rsa,
                                      encryption_padding, signature_padding,
                                      digest_algorithm));
        this->_K.reset(new PublicKey(*this->_k,
                                     envelope_cipher, envelope_mode));

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);
      }
#endif

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
              throw Error(
                elle::sprintf("unable to allocate a keypair generation "
                              "context: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            // Initialise the context for key generation.
            if (::EVP_PKEY_keygen_init(this->_context.get()) <= 0)
              throw Error(
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
        generate(elle::Natural32 const length,
                 Padding const encryption_padding,
                 Padding const signature_padding,
                 Oneway const digest_algorithm,
                 Cipher const envelope_cipher,
                 Mode const envelope_mode)
        {
          ELLE_TRACE_FUNCTION(length,
                              encryption_padding, signature_padding,
                              digest_algorithm,
                              envelope_cipher, envelope_mode);

          if ((length % 8) != 0)
            throw Error(
              elle::sprintf("the keypair length must be a multiple of 8"));

          // Make sure the cryptographic system is set up.
          cryptography::require();

          // Retrieve the key pair generation context.
          static ::EVP_PKEY_CTX* context = _context();

          // Set the key length in the keypair generation context.
          if (::EVP_PKEY_CTX_set_rsa_keygen_bits(context, length) <= 0)
            throw Error(
              elle::sprintf("unable to set the length of the keypair to "
                            "be generated: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          ::EVP_PKEY* key = nullptr;

          // Generate the EVP key.
          if (::EVP_PKEY_keygen(context, &key) <= 0)
            throw Error(
              elle::sprintf("unable to generate a keypair: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

          ELLE_ASSERT_NEQ(key, nullptr);

          // Instanciate both a RSA public and private key based on the
          // EVP_PKEY.
          PrivateKey k(key,
                       encryption_padding, signature_padding,
                       digest_algorithm);
          PublicKey K(k,
                      envelope_cipher, envelope_mode);

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(key);

          return (KeyPair(std::move(K), std::move(k)));
        }
      }
    }
  }
}
