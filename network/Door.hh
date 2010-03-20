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
// updated       julien quintard   [fri mar 19 16:55:05 2010]
//

///
/// this very special include is required as Link needs to know Door/Gate
/// while Door/Gate inherit Link. including Link.hh normally makes moc,
/// the QT meta object compiler, unable to detect the QObject classes.
///
/// therefore, Link.hh is not included when moc processes a header file.
///
#ifndef Q_MOC_RUN
# include <elle/network/Link.hh>
#endif

#ifndef ELLE_NETWORK_DOOR_HH
#define ELLE_NETWORK_DOOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Concurrency.hh>

#include <elle/network/Network.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Link.hh>

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
      public Link
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
      Status		Send(const I,
			     const Identifier& = Identifier::Null);
      template <typename I>
      Status		Transmit(const I,
				 const Identifier& = Identifier::Null);
      template <typename I,
		typename O>
      Status		Call(const I,
			     O);
      template <typename O>
      Status		Receive(const Identifier&,
				O);
      template <typename I>
      Status		Reply(const I);

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
