//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue feb 23 18:15:07 2010]
//

#ifndef ELLE_NETWORK_DATA_HH
#define ELLE_NETWORK_DATA_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/package/Archive.hh>

namespace elle
{
  using namespace package;

  namespace network
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this type represents a message's data section which contains the
    /// arguments.
    ///
    typedef Archive             Data;

  }
}

#endif
