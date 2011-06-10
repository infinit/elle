//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Wall.cc
//
// created       julien quintard   [fri aug 14 12:57:57 2009]
// updated       julien quintard   [wed jun  1 11:32:33 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Wall.hh>

#include <etoile/configuration/Configuration.hh>

#include <etoile/user/User.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the wall by setting up the interface callbacks.
    ///
    elle::Status	Wall::Initialize()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method cleans the callbacks.
    ///
    elle::Status	Wall::Clean()
    {
      enter();

      // nothing to do.

      leave();
    }

  }
}
