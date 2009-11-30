//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Edge.hh
//
// created       julien quintard   [fri oct 16 01:43:37 2009]
// updated       julien quintard   [fri nov 27 05:11:28 2009]
//

#ifndef ELLE_NETWORK_EDGE_HH
#define ELLE_NETWORK_EDGE_HH

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
    /// this class connects two slots through a channel known as a bridge.
    ///
    class Edge:
      public Channel
    {
    public:
      // XXX
    };

  }
}

#endif
