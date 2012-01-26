//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [fri may 20 19:32:16 2011]
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
      const elle::Character             Component[] = "remote";

      ///
      /// this global variable makes the machine i.e the current host
      /// accessible from every instance of the implementation.
      ///
      Machine*                          Remote::Computer = NULL;

    }
  }
}
