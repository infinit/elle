#include <cryptography/KeyPair.hh>
#include <cryptography/PublicKey.hh>
#include <cryptography/PrivateKey.hh>
#include <cryptography/SecretKey.hh>
#include <cryptography/Seed.hh>
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

#if defined(ELLE_CRYPTOGRAPHY_ROTATION)
    KeyPair::KeyPair(Seed const& seed)
    {
      switch (seed.cryptosystem())
      {
        case Cryptosystem::rsa:
        {
          // Generate a RSA key pair.
          std::pair<rsa::PublicKey, rsa::PrivateKey> pair =
            rsa::keypair::generate(seed.implementation());

          // Construct high-level public and private keys.
          std::unique_ptr<publickey::Interface> K(
            new rsa::PublicKey(std::move(pair.first)));
          std::unique_ptr<privatekey::Interface> k(
            new rsa::PrivateKey(std::move(pair.second)));

          // Set both public and private keys.
          this->_K.reset(new PublicKey(std::move(K)));
          this->_k.reset(new PrivateKey(std::move(k)));
        }
        default:
          throw Exception(
            elle::sprintf("unknown or non-supported asymmetric "
                          "cryptosystem '%s'", seed.cryptosystem()));
      }
    }
#endif

    KeyPair::KeyPair(KeyPair const& other):
      elle::serialize::DynamicFormat<KeyPair>(other),
      _K(new PublicKey(*other._K)),
      _k(new PrivateKey(*other._k))
    {
    }

    KeyPair::KeyPair(KeyPair&& other):
      elle::serialize::DynamicFormat<KeyPair>(std::move(other)),
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

    Cryptosystem
    KeyPair::cryptosystem() const
    {
      ELLE_ASSERT_NEQ(this->_K, nullptr);
      ELLE_ASSERT_NEQ(this->_k, nullptr);
      ELLE_ASSERT_EQ(this->_K->cryptosystem(), this->_k->cryptosystem());

      return (this->_K->cryptosystem());
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
