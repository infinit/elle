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
// updated       julien quintard   [sun jun  5 23:38:20 2011]
//

#ifndef ELLE_NETWORK_SLOT_HH
#define ELLE_NETWORK_SLOT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <elle/standalone/Region.hh>

#include <elle/radix/Status.hh>

#include <elle/concurrency/Event.hh>

#include <elle/network/Socket.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Parcel.hh>
#include <elle/network/Port.hh>

#include <elle/idiom/Close.hh>
# include <QObject>
# include <QUdpSocket>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace radix;
  using namespace concurrency;

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
      Slot(const Socket::Mode);
      ~Slot();

      //
      // methods
      //
      Status		Create();
      Status		Create(const Port);

      Status		Write(const Address&,
			      const Packet&);
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
      // callbacks
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
