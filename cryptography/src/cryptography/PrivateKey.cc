#include <cryptography/PrivateKey.hh>
#include <cryptography/KeyPair.hh>
#include <cryptography/Seed.hh>
#include <cryptography/Code.hh>
#include <cryptography/Exception.hh>
#include <cryptography/oneway.hh>
#include <cryptography/rsa/PrivateKey.hh>

#include <elle/log.hh>
#include <elle/utility/Factory.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.PrivateKey");

namespace infinit
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/

    PrivateKey::PrivateKey()
    {
    }

    PrivateKey::PrivateKey(std::unique_ptr<privatekey::Interface>&&
                             implementation):
      _implementation(std::move(implementation))
    {
    }

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
    PrivateKey::PrivateKey(Seed const& seed)
    {
      switch (seed.cryptosystem())
      {
        case Cryptosystem::rsa:
        {
          // Deduce a RSA private key implementation based on the seed.
          this->_implementation.reset(
            new rsa::PrivateKey(
              rsa::privatekey::deduce(seed.implementation())));

          break;
        }
        default:
          throw Exception(
            elle::sprintf("unknown or non-supported asymmetric "
                          "cryptosystem '%s'", seed.cryptosystem()));
      }
    }
#endif

    PrivateKey::PrivateKey(PrivateKey const& other):
      _implementation(other._implementation->clone())
    {
    }

    PrivateKey::PrivateKey(PrivateKey&& other):
      _implementation(std::move(other._implementation))
    {
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(PrivateKey)
    {
    }

    /*--------.
    | Methods |
    `--------*/

    Clear
    PrivateKey::decrypt(Code const& code) const
    {
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("code: %x", code);

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->decrypt(code));
    }

    Signature
    PrivateKey::sign(Digest const& digest) const
    {
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("digest: %x", digest);

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->sign(digest));
    }

    Signature
    PrivateKey::sign(Plain const& plain) const
    {
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("plain: %x", plain);

      Digest digest = oneway::hash(plain, KeyPair::oneway_algorithm);

      return (this->sign(digest));
    }

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
    Seed
    PrivateKey::derive(Seed const& seed) const
    {
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("seed: %x", seed);

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->derive(seed));
    }

    Seed
    PrivateKey::rotate(Seed const& seed) const
    {
      ELLE_TRACE_METHOD("");
      ELLE_DUMP("seed: %x", seed);

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->rotate(seed));
    }
#endif

    Cryptosystem
    PrivateKey::cryptosystem() const
    {
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->cryptosystem());
    }

    elle::Natural32
    PrivateKey::size() const
    {
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->size());
    }

    elle::Natural32
    PrivateKey::length() const
    {
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->length());
    }

    privatekey::Interface const&
    PrivateKey::implementation() const
    {
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (*this->_implementation);
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    PrivateKey::operator ==(PrivateKey const& other) const
    {
      if (this == &other)
        return (true);

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);
      ELLE_ASSERT_NEQ(other._implementation, nullptr);

      return (*this->_implementation == *other._implementation);
    }

    elle::Boolean
    PrivateKey::operator <(PrivateKey const& other) const
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

    PrivateKey::PrivateKey(elle::serialization::SerializerIn& serializer)
    {
      this->serialize(serializer);
    }

    void
    PrivateKey::serialize(elle::serialization::Serializer& serializer)
    {
      serializer.serialize_forward(this->_implementation);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    PrivateKey::print(std::ostream& stream) const
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
    namespace privatekey
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

        factory.record<rsa::PrivateKey>(Cryptosystem::rsa);

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
