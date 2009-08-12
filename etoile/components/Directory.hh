//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Directory.hh
//
// created       julien quintard   [sun aug  9 16:30:51 2009]
// updated       julien quintard   [mon aug 10 01:07:01 2009]
//

#ifndef ETOILE_COMPONENTS_DIRECTORY_HH
#define ETOILE_COMPONENTS_DIRECTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Core.hh>
#include <etoile/hole/Hole.hh>

namespace etoile
{
  using namespace hole;

  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides methods for managing directories.
    ///
    class Directory
    {
    public:
      //
      // static methods
      //
      static Status	Load(const Address&,
			     Object&);

      static Status	Lookup(const Object&,
			       const String&,
			       Address&);
    };

  }
}

#endif
