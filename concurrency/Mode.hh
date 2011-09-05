//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Mode.hh
//
// created       julien quintard   [tue mar  9 11:03:08 2010]
// updated       julien quintard   [sat sep  3 23:29:46 2011]
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
