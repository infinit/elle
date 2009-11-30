//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Bridge.hh
//
// created       julien quintard   [fri oct 16 01:50:11 2009]
// updated       julien quintard   [fri nov 27 05:01:17 2009]
//

#ifndef ELLE_NETWORK_BRIDGE_HH
#define ELLE_NETWORK_BRIDGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Channel.hh>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class connects two sockets through a channel known as a bridge.
    ///
    class Bridge:
      public Channel
    {
    public:
      // XXX
    };

  }
}

#endif
