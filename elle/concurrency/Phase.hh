//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu apr  8 20:14:01 2010]
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
