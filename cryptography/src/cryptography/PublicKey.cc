#include <cryptography/PublicKey.hh>
#include <cryptography/KeyPair.hh>
#include <cryptography/Code.hh>
#include <cryptography/Exception.hh>
#include <cryptography/oneway.hh>
#include <cryptography/rsa/PublicKey.hh>

#include <elle/log.hh>
#include <elle/serialization/Serializer.hh>
#include <elle/utility/Factory.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.PublicKey");

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/

    PublicKey::PublicKey()
    {
    }

    PublicKey::PublicKey(std::unique_ptr<publickey::Interface>&&
                           implementation):
      _implementation(std::move(implementation))
    {
    }

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
    PublicKey::PublicKey(Seed const& seed)
    {
      switch (seed.cryptosystem())
      {
        case Cryptosystem::rsa:
        {
          // Deduce a RSA public key implementation based on the seed.
          this->_implementation.reset(
            new rsa::PublicKey(
              rsa::publickey::deduce(seed.implementation())));

          break;
        }
        default:
          throw Exception(
            elle::sprintf("unknown or non-supported asymmetric "
                          "cryptosystem '%s'", seed.cryptosystem()));
      }
    }
#endif

    PublicKey::PublicKey(PublicKey const& other):
      _implementation(other._implementation->clone())
    {
    }

    PublicKey::PublicKey(PublicKey&& other):
      _implementation(std::move(other._implementation))
    {
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(PublicKey)
    {
    }

    /*--------.
    | Methods |
    `--------*/

    Code
    PublicKey::encrypt(Plain const& plain) const
    {
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("plain: %x", plain);

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->encrypt(plain));
    }

    elle::Boolean
    PublicKey::verify(Signature const& signature,
                      Digest const& digest) const
    {
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("signature: %x", signature);
      ELLE_DUMP("digest: %x", digest);

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->verify(signature, digest));
    }

    elle::Boolean
    PublicKey::verify(Signature const& signature,
                      Plain const& plain) const
    {
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("signature: %x", signature);
      ELLE_DUMP("plain: %x", plain);

      Digest digest = oneway::hash(plain, KeyPair::oneway_algorithm);

      return (this->verify(signature, digest));
    }

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
    Seed
    PublicKey::rotate(Seed const& seed) const
    {
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("seed: %x", seed);

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->rotate(seed));
    }

    Seed
    PublicKey::derive(Seed const& seed) const
    {
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("seed: %x", seed);

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->derive(seed));
    }
#endif

    Cryptosystem
    PublicKey::cryptosystem() const
    {
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->cryptosystem());
    }

    elle::Natural32
    PublicKey::size() const
    {
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->size());
    }

    elle::Natural32
    PublicKey::length() const
    {
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->length());
    }

    publickey::Interface const&
    PublicKey::implementation() const
    {
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (*this->_implementation);
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    PublicKey::operator ==(PublicKey const& other) const
    {
      if (this == &other)
        return (true);

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);
      ELLE_ASSERT_NEQ(other._implementation, nullptr);

      return (*this->_implementation == *other._implementation);
    }

    elle::Boolean
    PublicKey::operator <(PublicKey const& other) const
    {
      if (this == &other)
        return (true);

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);
      ELLE_ASSERT_NEQ(other._implementation, nullptr);

      return (*this->_implementation < *other._implementation);
    }

    /*--------------.
    | Serialization |
    `--------------*/

    PublicKey::PublicKey(elle::serialization::SerializerIn& serializer)
    {
      this->serialize(serializer);
    }

    void
    PublicKey::serialize(elle::serialization::Serializer& serializer)
    {
      serializer.serialize_forward(this->_implementation);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    PublicKey::print(std::ostream& stream) const
    {
      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      this->_implementation->print(stream);
    }
  }
}

//
// ---------- Factory ---------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace publickey
    {
      /*-----------------.
      | Static Functions |
      `-----------------*/

      static
      elle::utility::Factory<Cryptosystem>
      _factory()
      {
        ELLE_DEBUG_FUNCTION("");

        elle::utility::Factory<Cryptosystem> factory;

        factory.record<rsa::PublicKey>(Cryptosystem::rsa);

        return (factory);
      }

      /*----------.
      | Functions |
      `----------*/

      elle::utility::Factory<Cryptosystem> const&
      factory()
      {
        static elle::utility::Factory<Cryptosystem> factory = _factory();

        return (factory);
      }
    }
  }
}
