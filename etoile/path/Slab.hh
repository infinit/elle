//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/path/Slab.hh
//
// created       julien quintard   [thu jun 16 10:25:17 2011]
// updated       julien quintard   [sun sep  4 18:10:59 2011]
//

#ifndef ETOILE_PATH_SLAB_HH
#define ETOILE_PATH_SLAB_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this type represents the component of a file system path, represented
    /// by a Route in Etoile.
    ///
    /// note that such a component may contain a version number indicator
    /// such as '%23' should '%[0-9]+' be the associated version number
    /// regular expression.
    ///
    typedef elle::String	Slab;

  }
}

#endif
