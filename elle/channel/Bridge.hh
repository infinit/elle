//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/channel/Bridge.hh
//
// created       julien quintard   [fri oct 16 01:50:11 2009]
// updated       julien quintard   [fri oct 16 15:25:38 2009]
//

#ifndef ELLE_CHANNEL_BRIDGE_HH
#define ELLE_CHANNEL_BRIDGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/channel/Channel.hh>

#include <QUdpSocket>

namespace elle
{
  namespace channel
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Bridge:
      public Channel
    {
    public:
      //
      // constructors & destructors
      //

      //
      // methods
      //
      Status		Open(const Options&);
      Status		Close();

      Status		Restrict(const Location&);
      Status		Connect(const Socket&);

      Status		Send(const Archive&,
			     const Location& = Location::Null);
      Status		Receive(Archive&,
				Location& = Location::Null);

      //
      // attributes
      //
      ::QUdpSocket*	socket;
      ::quint16		port;
    };

  }
}

#endif
