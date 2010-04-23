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
// updated       julien quintard   [thu apr 22 11:07:21 2010]
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
      static Status	Create();
      static Status	Load(const path::Way&);
      static Status	Lock(const context::Identifier&);
      static Status	Release(const context::Identifier&);
      static Status	Add(const context::Identifier&,
			    const path::Slice&,
			    const context::Identifier&);
      static Status	Lookup(const context::Identifier&,
			       const path::Slice&);
      static Status	Consult(const context::Identifier&,
				const kernel::Index&,
				const kernel::Size&);
      static Status	Rename(const context::Identifier&,
			       const path::Slice&,
			       const path::Slice&);
      static Status	Remove(const context::Identifier&,
			       const path::Slice&);
      static Status	Discard(const context::Identifier&);
      static Status	Store(const context::Identifier&);
      static Status	Destroy(const context::Identifier&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Directory.hh>
#include <etoile/context/Format.hh>

#include <etoile/user/User.hh>

#include <etoile/path/Path.hh>

#endif
