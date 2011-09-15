//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri aug 21 22:37:24 2009]
//

#ifndef NUCLEUS_PROTON_STATE_HH
#define NUCLEUS_PROTON_STATE_HH

namespace nucleus
{
  namespace proton
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// this enumeration is used to specify the state of an in-memory block.
    ///
    /// the dirty state indicates that the block has been modified.
    ///
    /// the consistent state indicates that the block has changed but the
    /// required adjustements have since been performed so that the block
    /// can be stored in that state.
    ///
    enum State
      {
	StateClean = 0,
	StateDirty,
	StateConsistent
      };

  }
}

#endif
