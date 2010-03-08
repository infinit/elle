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
// updated       julien quintard   [thu mar  4 16:47:15 2010]
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

    // XXX
    typedef core::Permissions	Permissions;
    typedef core::Version	Version;

    typedef Natural32		Identifier;

    typedef String		Name;
    typedef String		Value;

    typedef String		Path;

    typedef core::Subject	Subject;

    typedef std::list<String>	Report;

    class Metadata
    {
    public:
      // XXX
    };

  }
}

#endif
