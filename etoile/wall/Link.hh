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
// updated       julien quintard   [thu may  5 16:32:55 2011]
//

#ifndef ETOILE_WALL_LINK_HH
#define ETOILE_WALL_LINK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/path/Way.hh>

#include <etoile/context/Identifier.hh>

#include <etoile/components/Link.hh>

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
      static elle::Status	Create();
      static elle::Status	Load(const path::Way&);
      static elle::Status	Lock(const context::Identifier&);
      static elle::Status	Release(const context::Identifier&);
      static elle::Status	Bind(const context::Identifier&,
				     const path::Way&);
      static elle::Status	Resolve(const context::Identifier&);
      static elle::Status	Discard(const context::Identifier&);
      static elle::Status	Store(const context::Identifier&);
      static elle::Status	Destroy(const context::Identifier&);
    };

  }
}

#endif
