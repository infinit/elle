//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Slot.hh
//
// created       julien quintard   [wed feb  3 21:04:37 2010]
// updated       julien quintard   [fri mar 19 16:55:02 2010]
//

#ifndef ELLE_NETWORK_SLOT_HH
#define ELLE_NETWORK_SLOT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/concurrency/Concurrency.hh>

#include <elle/network/Socket.hh>
#include <elle/network/Port.hh>
#include <elle/network/Address.hh>
#include <elle/network/Message.hh>
#include <elle/network/Network.hh>
#include <elle/network/Data.hh>
#include <elle/network/Raw.hh>
#include <elle/network/Packet.hh>

#include <elle/idiom/Close.hh>
# include <QObject>
# include <QUdpSocket>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// a slot represents a non-connected socket, typically implemented
    /// through the UDP protocol.
    ///
    class Slot:
      public ::QObject,
      public Socket
    {
      Q_OBJECT;

    public:
      //
      // constructors & destructors
      //
      Slot();
      ~Slot();

      //
      // methods
      //
      Status		Create();
      Status		Create(const Port);

      template <typename I>
      Status		Send(const Address&,
			     const I&,
			     const Identifier& = Identifier::Null);
      template <typename I>
      Status		Transmit(const Address&,
				 const I&,
				 const Identifier& = Identifier::Null);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      ::QUdpSocket*	socket;
      Port		port;

    private slots:
      //
      // slots
      //
      void		Error(QAbstractSocket::SocketError);
      void		Fetch();
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Slot.hxx>

#endif

