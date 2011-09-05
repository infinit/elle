//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Phase.hh
//
// created       julien quintard   [thu apr  8 20:14:01 2010]
// updated       julien quintard   [sat sep  3 23:30:29 2011]
//

#ifndef ELLE_CONCURRENCY_PHASE_HH
#define ELLE_CONCURRENCY_PHASE_HH

namespace elle
{
  namespace concurrency
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// this enumeration defines the different phases when it comes to
    /// governing fibers, from initializing the fiber's environment, to
    /// saving its context, to restoring it later to finally cleaning it.
    ///
    enum Phase
      {
	PhaseInitialize,
	PhaseSave,
	PhaseRestore,
	PhaseClean
      };

  }
}

#endif
