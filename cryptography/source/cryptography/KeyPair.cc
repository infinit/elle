#include <cryptography/KeyPair.hh>
#include <cryptography/KeyPair.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/SecretKey.hh>
#include <cryptography/Seed.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>

#include <elle/types.hh>
#include <elle/Exception.hh>

#include <comet/Comet.hh>

#include <elle/log.hh>

#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <fcntl.h>

ELLE_LOG_COMPONENT("cryptography.KeyPair");

namespace infinit
{
  namespace cryptography
  {
    /*------------------.
    | Static Attributes |
    `------------------*/

    ::EVP_PKEY_CTX* KeyPair::Contexts::generate = nullptr;

    /*---------------.
    | Static Methods |
    `---------------*/

    void
    KeyPair::initialize()
    {
      ELLE_TRACE("initializing the keypair contexts");

      // Create the context for the RSA algorithm.
      if ((KeyPair::Contexts::generate =
           ::EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr)) == nullptr)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      // Initialise the context for key generation.
      if (::EVP_PKEY_keygen_init(KeyPair::Contexts::generate) <= 0)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      ELLE_TRACE("keypair contexts initialized");
    }

    void
    KeyPair::clean()
    {
      ELLE_TRACE("cleaning the keypair contexts");

      // Release the generation context.
      ::EVP_PKEY_CTX_free(KeyPair::Contexts::generate);

      ELLE_TRACE("keypair contexts cleaned");
    }

    KeyPair
    KeyPair::generate(elle::Natural32 const length) // XXX[add algorithm argument]
    {
      ELLE_TRACE_FUNCTION(length);

      // Make sure the cryptographic system is set up.
      cryptography::require();

      // Set the key length in the keypair generation context.
      if (::EVP_PKEY_CTX_set_rsa_keygen_bits(KeyPair::Contexts::generate,
                                             length) <= 0)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      ::EVP_PKEY* key{nullptr};

      // Generate the EVP key.
      if (::EVP_PKEY_keygen(KeyPair::Contexts::generate, &key) <= 0)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

      ELLE_ASSERT(key != nullptr);

      // Instanciate a keypair based on the EVP_PKEY and return it.
      KeyPair pair(key);

      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(key);

      return (pair);
    }

    /*-------------.
    | Construction |
    `-------------*/

    KeyPair::KeyPair()
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    KeyPair::KeyPair(PublicKey const& K,
                     PrivateKey const& k):
      _K(K),
      _k(k)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    KeyPair::KeyPair(::EVP_PKEY const* key):
      _K(key),
      _k(key)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    KeyPair::KeyPair(KeyPair const& pair):
      _K(pair._K),
      _k(pair._k)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(KeyPair)
    {
    }

    /*--------.
    | Methods |
    `--------*/

    /* XXX
    ///
    /// this method rotates a key pair based on a given seed.
    ///
    /// this seed will then be used---by people having access to it,
    /// and to the key modulus---in order to retrieve the public key.
    /// for more information, please refer to PrivateKey::Derive().
    ///
    elle::Status              KeyPair::Rotate(const Seed&             seed,
                                        KeyPair&                pair) const
    {
      ELLE_TRACE_METHOD(seed, pair);

      struct Scope
      {
        ::EVP_PKEY*  key;
        ::RSA*       rsa;

        Scope() : key(nullptr), rsa(nullptr) {}
        ~Scope() { ::EVP_PKEY_free(this->key); ::RSA_free(this->rsa); }
      } scope;

      // create an EVP key.
      if ((scope.key = ::EVP_PKEY_new()) == nullptr)
        throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // create a new RSA key.
      if ((scope.rsa = ::RSA_new()) == nullptr)
        throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // rotate the RSA key.
      if (comet::RSA_rotate(scope.rsa,
                            ::BN_num_bits(this->_K.key()->pkey.rsa->n),
                            seed.region.contents,
                            seed.region.size) <= 0)
        throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // assign the RSA key to the EVP's.
      if (::EVP_PKEY_assign_RSA(scope.key, scope.rsa) <= 0)
        throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // stop tracking.
      scope.rsa = nullptr;

      // create the rotated public key according to the EVP structure.
      if (pair._K.Create(scope.key) == elle::Status::Error)
        throw elle::Exception("unable to create the public key");

      // create the rotated private key according to the EVP structure.
      if (pair.k.Create(scope.key) == elle::Status::Error)
        throw elle::Exception("unable to create the private key");

      return elle::Status::Ok;
    }
    */

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    KeyPair::operator ==(KeyPair const& other) const
    {
      if (this == &other)
        return (true);

      return ((this->_K == other._K) && (this->_k == other._k));
    }

    /*----------.
    | Printable |
    `----------*/

    void
    KeyPair::print(std::ostream& stream) const
    {
      stream << "(" << this->_K << ", " << this->_k << ")";
    }
  }
}
