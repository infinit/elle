#ifndef NUCLEUS_NEUTRON_USER_HH
# define NUCLEUS_NEUTRON_USER_HH

#include <cryptography/PublicKey.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

namespace nucleus
{
  namespace neutron
  {

    /// The user identity does not benefit from a dedicated block though
    /// this could be useful in the future, especially in a completely
    /// decentralised environment. In our case however, the user identities
    /// are centralised in a database.
    class User
    {
      //
      // types
      //
    public:
      typedef cryptography::PublicKey Identity;
    };

  }
}

#endif
