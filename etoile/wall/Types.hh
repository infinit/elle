//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/wall/Types.hh
//
// created       julien quintard   [fri aug 14 17:41:49 2009]
// updated       julien quintard   [fri aug 14 17:42:06 2009]
//

#ifndef ETOILE_WALL_TYPES_HH
#define ETOILE_WALL_TYPES_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Core.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- types -----------------------------------------------------------
//

    typedef core::Permissions	Permissions;
    typedef core::Version	Version;

    typedef Natural32		Identifier;

    typedef String		Name;
    typedef String		Value;

    typedef String		Path;

    typedef core::Subject	Subject;
  }
}

#endif
