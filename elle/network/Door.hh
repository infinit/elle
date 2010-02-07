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
// updated       julien quintard   [sun feb  7 02:42:58 2010]
//

#ifndef ELLE_NETWORK_DOOR_HH
#define ELLE_NETWORK_DOOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Socket.hh>
#include <elle/network/Environment.hh>
#include <elle/network/Network.hh>

#include <QObject>
#include <QLocalSocket>

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
      ::QObject,
      Socket
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
      Status		Disconnect();

      Status		Send(const Message&);

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
      void		Read();
    };

  }
}

#endif
