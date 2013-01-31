#include <cryptography/PublicKey.hh>
#include <cryptography/Code.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/rsa/PublicKey.hh>

#include <elle/log.hh>
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
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    PublicKey::PublicKey(std::unique_ptr<publickey::Interface>&&
                           implementation):
      _implementation(std::move(implementation))
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    PublicKey::PublicKey(PublicKey const& other):
      _implementation(other._implementation->clone())
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    PublicKey::PublicKey(PublicKey&& other):
      _implementation(std::move(other._implementation))
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(PublicKey)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    /*--------.
    | Methods |
    `--------*/

    Code
    PublicKey::encrypt(Plain const& plain) const
    {
      ELLE_TRACE_METHOD(plain);

      ELLE_ASSERT(this->_implementation != nullptr);

      return (this->_implementation->encrypt(plain));
    }

    elle::Boolean
    PublicKey::verify(Signature const& signature,
                      Plain const& plain) const
    {
      ELLE_TRACE_METHOD(signature, plain);

      ELLE_ASSERT(this->_implementation != nullptr);

      return (this->_implementation->verify(signature, plain));
    }

    Clear
    PublicKey::decrypt(Code const& code) const
    {
      ELLE_TRACE_METHOD(code);

      ELLE_ASSERT(this->_implementation != nullptr);

      return (this->_implementation->decrypt(code));
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    PublicKey::operator ==(PublicKey const& other) const
    {
      if (this == &other)
        return (true);

      ELLE_ASSERT(this->_implementation != nullptr);
      ELLE_ASSERT(other._implementation != nullptr);

      return (*this->_implementation == *other._implementation);
    }

    elle::Boolean
    PublicKey::operator <(PublicKey const& other) const
    {
      if (this == &other)
        return (true);

      ELLE_ASSERT(this->_implementation != nullptr);
      ELLE_ASSERT(other._implementation != nullptr);

      return (*this->_implementation < *other._implementation);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    PublicKey::print(std::ostream& stream) const
    {
      ELLE_ASSERT(this->_implementation != nullptr);

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
        ELLE_DEBUG_FUNCTION("");

        static elle::utility::Factory<Cryptosystem> factory = _factory();

        return (factory);
      }
    }
  }
}
