//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/channel/Edge.hh
//
// created       julien quintard   [fri oct 16 01:43:37 2009]
// updated       julien quintard   [fri oct 16 03:06:42 2009]
//

#ifndef ELLE_CHANNEL_EDGE_HH
#define ELLE_CHANNEL_EDGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/channel/Channel.hh>

#include <QLocalSocket>

namespace elle
{
  namespace channel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class abstracts a local communication channel.
    ///
    class Edge:
      public Channel
    {
    public:
      //
      // attributes
      //
      ::QLocalSocket	socket;
    };

  }
}

#endif
