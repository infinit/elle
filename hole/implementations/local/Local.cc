//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/local/Local.cc
//
// created       julien quintard   [thu may 12 10:27:04 2011]
// updated       julien quintard   [wed aug 31 13:43:34 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/local/Local.hh>

#include <Infinit.hh>

namespace hole
{
  namespace implementations
  {
    namespace local
    {

//
// ---------- definitions -----------------------------------------------------
//

      ///
      /// this value defines the component's name.
      ///
      const elle::Character		Component[] = "local";

      ///
      /// XXX
      ///
      Machine*				Local::Computer = NULL;

    }
  }
}
