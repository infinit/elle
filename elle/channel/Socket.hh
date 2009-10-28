//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/channel/Socket.hh
//
// created       julien quintard   [fri oct 16 05:17:01 2009]
// updated       julien quintard   [fri oct 16 05:19:30 2009]
//

#ifndef ELLE_CHANNEL_SOCKET_HH
#define ELLE_CHANNEL_SOCKET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/channel/Location.hh>

namespace elle
{
  namespace channel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the communication path to a remote process
    /// and is composed of both a network location and a port.
    ///
    class Socket
    {
    public:
      //
      // attributes
      //
      Location		location;
      Natural16		port;
    };

  }
}

#endif
