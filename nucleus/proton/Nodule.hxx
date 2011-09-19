//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [sun sep 18 14:35:06 2011]
//

#ifndef NUCLEUS_PROTON_NODULE_HXX
#define NUCLEUS_PROTON_NODULE_HXX

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
    template <typename V>
    Nodule<V>::Nodule(const Type				type):
      type(type),
      parent(NULL),
      left(NULL),
      right(NULL)
    {
    }

  }
}

#endif
