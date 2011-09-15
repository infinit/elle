//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri aug 14 15:36:23 2009]
//

#ifndef ETOILE_WALL_OBJECT_HH
#define ETOILE_WALL_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/path/Chemin.hh>

#include <etoile/gear/Identifier.hh>

#include <etoile/miscellaneous/Information.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides general-purpose methods for manipulating objects.
    ///
    /// these are very useful when the caller does not know the genre of
    /// the object i.e file, directory or link.
    ///
    class Object
    {
    public:
      //
      // static methods
      //
      static elle::Status	Load(const path::Chemin&,
				     gear::Identifier&);

      static elle::Status	Lock(const gear::Identifier&);
      static elle::Status	Release(const gear::Identifier&);
      static elle::Status	Information(const gear::Identifier&,
					    miscellaneous::Information&);

      static elle::Status	Discard(const gear::Identifier&);
      static elle::Status	Store(const gear::Identifier&);
      static elle::Status	Destroy(const gear::Identifier&);
      static elle::Status	Purge(const gear::Identifier&);
    };

  }
}

#endif
