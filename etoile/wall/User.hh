//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/User.hh
//
// created       julien quintard   [thu mar  4 16:06:41 2010]
// updated       julien quintard   [thu mar  4 16:49:00 2010]
//

#ifndef ETOILE_WALL_USER_HH
#define ETOILE_WALL_USER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class User
    {
    public:
      //
      // static methods
      //
      static Status	Identify(const PublicKey&,
				 const String&);
      static Status	Authenticate(const Digest&);
      static Status	Connect(const String&);
    };

  }
}

#endif
