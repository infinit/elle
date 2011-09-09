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
// updated       julien quintard   [thu sep  8 17:22:16 2011]
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
      /// this variable is set global in order for any instance of the
      /// local implementation to retrieve the machine i.e the current
      /// host.
      ///
      Machine*				Local::Computer = NULL;

    }
  }
}
