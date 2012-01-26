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

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

namespace elle
{
  using namespace core;

  namespace concurrency
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this enumeration defines the type of access to a lock.
    ///
    typedef Natural8            Mode;

//
// ---------- constants -------------------------------------------------------
//

    ///
    /// default mode values.
    ///
    const Mode                  ModeRead = 0x1;
    const Mode                  ModeWrite = 0x2;

  }
}

#endif
