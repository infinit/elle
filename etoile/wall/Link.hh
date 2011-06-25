//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Link.hh
//
// created       julien quintard   [fri aug 14 15:36:23 2009]
// updated       julien quintard   [sat jun 25 14:37:48 2011]
//

#ifndef ETOILE_WALL_LINK_HH
#define ETOILE_WALL_LINK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/path/Chemin.hh>
#include <etoile/path/Way.hh>

#include <etoile/gear/Identifier.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for managing links.
    ///
    class Link
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
      static elle::Status	Bind(const gear::Identifier&,
				     const path::Way&);
      static elle::Status	Resolve(const gear::Identifier&,
					path::Way&);

      static elle::Status	Discard(const gear::Identifier&);
      static elle::Status	Store(const gear::Identifier&);
      static elle::Status	Destroy(const gear::Identifier&);
      static elle::Status	Purge(const gear::Identifier&);
    };

  }
}

#endif
