#include <cryptography/factory.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>

#include <elle/assert.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.factory");

namespace infinit
{
  namespace cryptography
  {
    namespace factory
    {
      namespace setup
      {
        /*----------.
        | Functions |
        `----------*/

        static
        elle::utility::Factory<Cryptosystem> const*
        _K()
        {
          elle::utility::Factory<Cryptosystem>* factory =
            new elle::utility::Factory<Cryptosystem>;

          ELLE_TRACE("setting up the cryptographic public key factory");

          factory->record<rsa::PublicKey>(Cryptosystem::rsa);

          return (factory);
        }

        static
        elle::utility::Factory<Cryptosystem> const*
        _k()
        {
          elle::utility::Factory<Cryptosystem>* factory =
            new elle::utility::Factory<Cryptosystem>;

          ELLE_TRACE("setting up the cryptographic private key factory");

          factory->record<rsa::PrivateKey>(Cryptosystem::rsa);

          return (factory);
        }
      }

      /*----------.
      | Functions |
      `----------*/

      elle::utility::Factory<Cryptosystem> const&
      K()
      {
        static elle::utility::Factory<Cryptosystem> const* factory =
          setup::_K();

        ELLE_ASSERT(factory != nullptr);

        return (*factory);
      }

      elle::utility::Factory<Cryptosystem> const&
      k()
      {
        static elle::utility::Factory<Cryptosystem> const* factory =
          setup::_k();

        ELLE_ASSERT(factory != nullptr);

        return (*factory);
      }
    }
  }
}
