//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Directory.hh
//
// created       julien quintard   [fri aug 14 15:36:23 2009]
// updated       julien quintard   [mon may  3 15:48:13 2010]
//

#ifndef ETOILE_WALL_DIRECTORY_HH
#define ETOILE_WALL_DIRECTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/path/Way.hh>

#include <etoile/context/Identifier.hh>

#include <etoile/kernel/Index.hh>
#include <etoile/kernel/Size.hh>

#include <etoile/components/Directory.hh>

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
      static elle::Status	Create();
      static elle::Status	Load(const path::Way&);
      static elle::Status	Lock(const context::Identifier&);
      static elle::Status	Release(const context::Identifier&);
      static elle::Status	Add(const context::Identifier&,
				    const path::Slice&,
				    const context::Identifier&);
      static elle::Status	Lookup(const context::Identifier&,
				       const path::Slice&);
      static elle::Status	Consult(const context::Identifier&,
					const kernel::Index&,
					const kernel::Size&);
      static elle::Status	Rename(const context::Identifier&,
				       const path::Slice&,
				       const path::Slice&);
      static elle::Status	Remove(const context::Identifier&,
				       const path::Slice&);
      static elle::Status	Discard(const context::Identifier&);
      static elle::Status	Store(const context::Identifier&);
      static elle::Status	Destroy(const context::Identifier&);
    };

  }
}

#endif
