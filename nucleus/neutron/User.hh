#ifndef NUCLEUS_NEUTRON_USER_HH
# define NUCLEUS_NEUTRON_USER_HH

#include <elle/cryptography/PublicKey.hh>

namespace nucleus
{
  namespace neutron
  {

    /// XXX[note that user identities do not benefit from a dedicated
    ///     block though this could be useful in a completely decentralised
    ///     environment. in our case however, the user identities are
    ///     centralised in a database.]
    class User
    {
      //
      // types
      //
    public:
      typedef elle::cryptography::PublicKey Identity;
    };

  }
}

#endif
