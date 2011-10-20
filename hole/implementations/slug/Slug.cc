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

#include <hole/implementations/slug/Slug.hh>

#include <hole/Hole.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

//
// ---------- definitions -----------------------------------------------------
//

      ///
      /// this value defines the component's name.
      ///
      const elle::Character		Component[] = "slug";

      ///
      /// XXX
      ///
      Machine*				Slug::Computer = NULL;

    }
  }
}
