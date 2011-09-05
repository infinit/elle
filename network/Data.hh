//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Data.hh
//
// created       julien quintard   [tue feb 23 18:15:07 2010]
// updated       julien quintard   [sun sep  4 12:01:47 2011]
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
    typedef Archive		Data;

  }
}

#endif
