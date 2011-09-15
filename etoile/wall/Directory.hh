//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri aug 14 15:36:23 2009]
//

#ifndef ETOILE_WALL_DIRECTORY_HH
#define ETOILE_WALL_DIRECTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/path/Chemin.hh>
#include <etoile/path/Slab.hh>

#include <etoile/gear/Identifier.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides an interface for manipulating directories.
    ///
    class Directory
    {
    public:
      //
      // static methods
      //
      static elle::Status	Create(gear::Identifier&);
      static elle::Status	Load(const path::Chemin&,
				     gear::Identifier&);

      static elle::Status	Lock(const gear::Identifier&);
      static elle::Status	Release(const gear::Identifier&);
      static elle::Status	Add(const gear::Identifier&,
				    const path::Slab&,
				    const gear::Identifier&);
      static elle::Status	Lookup(const gear::Identifier&,
				       const path::Slab&,
				       nucleus::Entry*&);
      static elle::Status	Consult(const gear::Identifier&,
					const nucleus::Index&,
					const nucleus::Size&,
					nucleus::Range<nucleus::Entry>&);
      static elle::Status	Rename(const gear::Identifier&,
				       const path::Slab&,
				       const path::Slab&);
      static elle::Status	Remove(const gear::Identifier&,
				       const path::Slab&);

      static elle::Status	Discard(const gear::Identifier&);
      static elle::Status	Store(const gear::Identifier&);
      static elle::Status	Destroy(const gear::Identifier&);
      static elle::Status	Purge(const gear::Identifier&);
    };

  }
}

#endif
