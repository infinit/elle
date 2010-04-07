//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/wall/Link.hh
//
// created       julien quintard   [fri aug 14 15:36:23 2009]
// updated       julien quintard   [sat apr  3 23:34:38 2010]
//

#ifndef ETOILE_WALL_LINK_HH
#define ETOILE_WALL_LINK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/path/Way.hh>

#include <etoile/context/Identifier.hh>

#include <etoile/kernel/Offset.hh>
#include <etoile/kernel/Size.hh>

#include <etoile/components/Link.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Link
    {
    public:
      //
      // static methods
      //
      static Status	Load(const path::Way&);
      static Status	Lock(const context::Identifier&);
      static Status	Release(const context::Identifier&);
      static Status	Create(const context::Identifier&,
			       const path::Slice&,
			       const context::Identifier&);
      static Status	Bind(const context::Identifier&,
			     const path::Way&);
      static Status	Resolve(const context::Identifier&);
      static Status	Store(const context::Identifier&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Link.hh>

#include <etoile/user/User.hh>

#endif
