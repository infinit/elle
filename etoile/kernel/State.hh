//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/State.hh
//
// created       julien quintard   [fri aug 21 22:37:24 2009]
// updated       julien quintard   [fri apr  2 12:14:50 2010]
//

#ifndef ETOILE_KERNEL_STATE_HH
#define ETOILE_KERNEL_STATE_HH

namespace etoile
{
  namespace kernel
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// this enumeration is used to specify if an block has been modified;
    /// hence update needs to be pushed into the hole.
    ///
    enum State
      {
	StateClean = 0,
	StateDirty
      };

  }
}

#endif
