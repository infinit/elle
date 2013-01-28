#ifndef INFINIT_CRYPTOGRAPHY_FACTORY_HH
# define INFINIT_CRYPTOGRAPHY_FACTORY_HH

# include <cryptography/Cryptosystem.hh>

# include <elle/utility/Factory.hh>

namespace infinit
{
  namespace cryptography
  {
    namespace factory
    {
      /*----------.
      | Functions |
      `----------*/

      /// Return the factory capable of building public key instances.
      elle::utility::Factory<Cryptosystem> const&
      K();
      /// Return the factory capable of building private key instances.
      elle::utility::Factory<Cryptosystem> const&
      k();
    }
  }
}

#endif
