//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Remote.cc
//
// created       julien quintard   [fri may 20 19:32:16 2011]
// updated       julien quintard   [wed aug 31 13:43:22 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Remote.hh>

#include <hole/Hole.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

//
// ---------- definitions -----------------------------------------------------
//

      ///
      /// this value defines the component's name.
      ///
      const elle::Character		Component[] = "remote";

      ///
      /// XXX
      ///
      Machine*				Remote::Computer = NULL;

    }
  }
}
