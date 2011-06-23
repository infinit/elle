//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/State.hh
//
// created       julien quintard   [wed jun 22 13:46:06 2011]
// updated       julien quintard   [wed jun 22 13:47:47 2011]
//

#ifndef ETOILE_GEAR_STATE_HH
#define ETOILE_GEAR_STATE_HH

namespace etoile
{
  namespace gear
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// this enumeration defines the various states related to a context.
    ///
    enum State
      {
	StateUnknown,
	StateInitialized,
	StateModified,
	StateStored,
	StateDestroyed,
	StateCleaned
      };

  }
}

#endif
