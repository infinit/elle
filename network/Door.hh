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
// updated       julien quintard   [thu apr  1 15:41:02 2010]
//

///
/// this very special include is required as Channel needs to know Door/Gate
/// while Door/Gate inherit Channel. including Channel.hh normally makes moc,
/// the QT meta object compiler, unable to detect the QObject classes.
///
/// therefore, Channel.hh is not included when moc processes a header file.
///
#ifndef Q_MOC_RUN
# include <elle/network/Channel.hh>
#endif

#ifndef ELLE_NETWORK_DOOR_HH
#define ELLE_NETWORK_DOOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Concurrency.hh>

#include <elle/network/Network.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Channel.hh>

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
    /// through a lane.
    ///
    /// such doors are often implemented on the system through pipes
    /// or local domain sockets.
    ///
    class Door:
      public ::QObject,
      public Channel
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
      Status		Create(const Socket::Mode = Socket::ModeAsynchronous);
      Status		Create(::QLocalSocket*,
			       const Socket::Mode);

      Status		Connect(const String&);
      Status		Disconnect();

      Status		Read(Parcel*&);

      template <typename I>
      Status		Send(const I,
			     const Event& = Event::Null);
      template <typename I>
      Status		Transmit(const I,
				 const Event& = Event::Null);
      template <typename I,
		typename O>
      Status		Call(const I,
			     O);
      template <typename O>
      Status		Receive(const Event&,
				O);
      template <typename I>
      Status		Reply(const I);

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
      ::QLocalSocket*	socket;

      Region*		buffer;
      Natural32		offset;

      //
      // slots
      //
    private slots:
      void		_error(const QLocalSocket::LocalSocketError);
      void		_fetch();
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Door.hxx>

#endif
