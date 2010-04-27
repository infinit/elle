//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/network/Slot.hh
//
// created       julien quintard   [wed feb  3 21:04:37 2010]
// updated       julien quintard   [mon apr 26 18:31:31 2010]
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

#include <elle/Manifest.hh>

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

      Status		Read(Parcel*&);

      template <typename I>
      Status		Send(const Address&,
			     const I&,
			     const Event& = Event::Null);
      template <typename I>
      Status		Transmit(const Address&,
				 const I&,
				 const Event& = Event::Null);

      //
      // entrances
      //
      Status		Error(const String&);
      Status		Fetch();

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
      void		_error(const QAbstractSocket::SocketError);
      void		_fetch();
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Slot.hxx>

#endif

