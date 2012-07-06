//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [fri aug 12 14:03:04 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/cirkle/Cirkle.hh>

#include <hole/Hole.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- definitions -----------------------------------------------------
//

      ///
      /// this value defines the component's name.
      ///
      const elle::Character             Component[] = "cirkle";

      ///
      /// XXX
      ///
      Machine*                          Cirkle::Computer = nullptr;

    }
  }
}
