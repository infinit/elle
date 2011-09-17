//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri sep 16 21:35:38 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Nodule.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Nodule::Nodule(const Type					type):
      type(type),
      parent(NULL),
      left(NULL),
      right(NULL)
    {
    }

  }
}
