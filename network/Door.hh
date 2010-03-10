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
// updated       julien quintard   [wed mar 10 20:42:57 2010]
//

#ifndef ELLE_NETWORK_DOOR_HH
#define ELLE_NETWORK_DOOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Concurrency.hh>

#include <elle/network/Socket.hh>
#include <elle/network/Network.hh>
#include <elle/network/Identifier.hh>
#include <elle/network/Packet.hh>

#include <elle/idiom/Close.hh>
# include <QObject>
# include <QLocalSocket>
#include <elle/idiom/Open.hh>

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
      static const Natural32		Capacity;

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
      Status		Disconnect();

      template <typename I>
      Status		Send(const I&,
			     const Identifier& = Identifier::Null);
      template <typename I>
      Status		Transmit(const I&,
				 const Identifier& = Identifier::Null);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      ::QLocalSocket*	socket;

      Region*		buffer;
      Natural32		offset;

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
      void		Fetch();
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Door.hxx>

#endif
