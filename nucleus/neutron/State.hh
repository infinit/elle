//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/State.hh
//
// created       julien quintard   [fri aug 21 22:37:24 2009]
// updated       julien quintard   [thu jun 16 22:11:18 2011]
//

#ifndef NUCLEUS_NEUTRON_STATE_HH
#define NUCLEUS_NEUTRON_STATE_HH

namespace nucleus
{
  namespace neutron
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// this enumeration is used to specify if an block has been modified;
    /// hence update needs to be pushed into the storage layer.
    ///
    enum State
      {
	StateClean = 0,
	StateDirty,
	StateUpdated,
	StateDying
      };

  }
}

#endif
