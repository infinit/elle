#include <cryptography/KeyPair.hh>
#include <cryptography/PublicKey.hh>
#include <cryptography/PrivateKey.hh>
#include <cryptography/SecretKey.hh>
#include <cryptography/Seed.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/Exception.hh>
#include <cryptography/rsa/keypair.hh>

#include <elle/log.hh>
#include <elle/types.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.KeyPair");

namespace infinit
{
  namespace cryptography
  {
    /*---------------.
    | Static Methods |
    `---------------*/

    KeyPair
    KeyPair::generate(Cryptosystem const cryptosystem,
                      elle::Natural32 const length)
    {
      ELLE_TRACE_FUNCTION(cryptosystem, length);

      switch (cryptosystem)
      {
        case Cryptosystem::rsa:
        {
          // Generate a RSA key pair.
          std::pair<rsa::PublicKey, rsa::PrivateKey> pair =
            rsa::keypair::generate(length);

          // Construct high-level public and private keys.
          std::unique_ptr<publickey::Interface> K(
            new rsa::PublicKey(std::move(pair.first)));
          std::unique_ptr<privatekey::Interface> k(
            new rsa::PrivateKey(std::move(pair.second)));

          // Construct a key pair based on both public and private key.
          return (KeyPair(PublicKey(std::move(K)), PrivateKey(std::move(k))));
        }
        default:
          throw Exception(elle::sprintf("unknown or non-supported asymmetric "
                                        "cryptosystem '%s'", cryptosystem));
      }
      elle::unreachable();
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
      _K(new PublicKey(K)),
      _k(new PrivateKey(k))
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    KeyPair::KeyPair(PublicKey&& K,
                     PrivateKey&& k):
      _K(new PublicKey(std::move(K))),
      _k(new PrivateKey(std::move(k)))
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    KeyPair::KeyPair(KeyPair const& other):
      _K(new PublicKey(*other._K)),
      _k(new PrivateKey(*other._k))
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    KeyPair::KeyPair(KeyPair&& other):
      _K(std::move(other._K)),
      _k(std::move(other._k))
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    KeyPair::KeyPair(Cryptosystem const cryptosystem,
                     Seed const& seed)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();

      // XXX
      /*
//       struct Scope
//       {
//         ::EVP_PKEY*  key;
//         ::RSA*       rsa;

//         Scope() : key(nullptr), rsa(nullptr) {}
//         ~Scope() { ::EVP_PKEY_free(this->key); ::RSA_free(this->rsa); }
//       } scope;

      ::EVP_PKEY* key = nullptr;

      // Allocate an EVP key.
      if ((key = ::EVP_PKEY_new()) == nullptr)
        throw Exception("unable to allocate the EVP_PKEY: %s",
                        ::ERR_error_string(ERR_get_error(), nullptr));

      // Create a new RSA key.
      if ((scope.rsa = ::RSA_new()) == nullptr)
        throw Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // rotate the RSA key.
      if (comet::RSA_rotate(scope.rsa,
                            ::BN_num_bits(this->_K->key()->pkey.rsa->n),
                            seed.region.contents,
                            seed.region.size) <= 0)
        throw Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // assign the RSA key to the EVP's.
      if (::EVP_PKEY_assign_RSA(scope.key, scope.rsa) <= 0)
        throw Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // stop tracking.
      scope.rsa = nullptr;

      // create the rotated public key according to the EVP structure.
      if (pair._K->Create(scope.key) == elle::Status::Error)
        throw Exception("unable to create the public key");

      // create the rotated private key according to the EVP structure.
      if (pair.k.Create(scope.key) == elle::Status::Error)
        throw Exception("unable to create the private key");
      */
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(KeyPair)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    /*--------.
    | Methods |
    `--------*/

    PublicKey const&
    KeyPair::K() const
    {
      ELLE_ASSERT(this->_K != nullptr);

      return (*this->_K);
    }

    PrivateKey const&
    KeyPair::k() const
    {
      ELLE_ASSERT(this->_k != nullptr);

      return (*this->_k);
    }

    elle::Natural32
    KeyPair::size() const
    {
      ELLE_ASSERT(this->_K != nullptr);

      return (this->_K->size());
    }

    elle::Natural32
    KeyPair::length() const
    {
      ELLE_ASSERT(this->_K != nullptr);

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

      ELLE_ASSERT(this->_K != nullptr);
      ELLE_ASSERT(this->_k != nullptr);

      return ((*this->_K == *other._K) && (*this->_k == *other._k));
    }

    /*----------.
    | Printable |
    `----------*/

    void
    KeyPair::print(std::ostream& stream) const
    {
      ELLE_ASSERT(this->_K != nullptr);
      ELLE_ASSERT(this->_k != nullptr);

      stream << "(" << *this->_K << ", " << *this->_k << ")";
    }
  }
}
