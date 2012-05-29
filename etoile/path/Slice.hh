//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri apr  2 01:29:28 2010]
//

#ifndef ETOILE_PATH_SLICE_HH
#define ETOILE_PATH_SLICE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this type represents the name of a file system path item.
    ///
    /// for instance, the path /teton/avale/suce/ is composed of
    /// three slices: 'teton', 'avale' and 'suce'.
    ///
    typedef elle::String        Slice;

  }
}

#endif
