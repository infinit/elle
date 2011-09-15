//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue mar  9 11:03:08 2010]
//

#ifndef ELLE_CONCURRENCY_MODE_HH
#define ELLE_CONCURRENCY_MODE_HH

namespace elle
{
  namespace concurrency
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// this enumeration defines the type of access to a lock.
    ///
    /// \todo XXX is it really used?
    ///
    enum Mode
      {
	ModeRead,
	ModeWrite
      };

  }
}

#endif
