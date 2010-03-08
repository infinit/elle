//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Door.hh
//
// created       julien quintard   [thu feb  4 14:42:14 2010]
// updated       julien quintard   [fri mar  5 21:14:16 2010]
//

#ifndef ELLE_NETWORK_DOOR_HH
#define ELLE_NETWORK_DOOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <QObject>
#include <QLocalSocket>

#include <elle/network/Socket.hh>
#include <elle/network/Network.hh>
#include <elle/network/Identifier.hh>
#include <elle/network/Packet.hh>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a socket locally connected to another door
    /// through a bridge.
    ///
    /// such doors are often implemented on the system through pipes
    /// or local domain sockets.
    ///
    class Door:
      public ::QObject,
      public Socket
    {
      Q_OBJECT;

    public:
      //
      // constants
      //
      static const Natural32		Timeout;

      //
      // constructors & destructors
      //
      Door();
      ~Door();

      //
      // methods
      //
      Status		Create();
      Status		Create(::QLocalSocket*);

      Status		Connect(const String&);
      Status		Read(Packet&);
      Status		Disconnect();

      template <typename I>
      Status		Send(const I&,
			     const Identifier& = Identifier::Null);
      template <typename I>
      Status		Transmit(const I&,
				 const Identifier& = Identifier::Null);
      template <typename O>
      Status		Receive(const Identifier&,
				O);
      template <typename I,
		typename O>
      Status		Request(const I&,
				O&);
      template <typename I,
		typename O>
      Status		Call(const I&,
			     O);
      template <typename I>
      Status		Reply(const I&);
      template <typename I>
      Status		Return(const I&);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      ::QLocalSocket*	socket;

      //
      // signals
      //
    signals:
      void		Disconnected();

      //
      // slots
      //
    private slots:
      void		Error(QLocalSocket::LocalSocketError);
      void		Deliver();
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Door.hxx>

#endif
