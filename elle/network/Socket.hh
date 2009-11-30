//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Socket.hh
//
// created       julien quintard   [fri oct 16 05:17:01 2009]
// updated       julien quintard   [mon nov 30 13:17:38 2009]
//

#ifndef ELLE_NETWORK_SOCKET_HH
#define ELLE_NETWORK_SOCKET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/archive/Archive.hh>

#include <elle/network/Address.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Location.hh>

#include <QObject>
#include <QUdpSocket>

namespace elle
{
  using namespace core;
  using namespace archive;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class is the base class for socket-specific functionoids.
    ///
    class SocketDispatcher
    {
    public:
      //
      // methods
      //
      virtual Status		Trigger(const Location&,
					Archive&) = 0;
    };

    ///
    /// this class represents a remote socket.
    ///
    /// \todo eventually this class could wrap UDP and TCP sockets.
    ///
    class Socket:
      public ::QObject,
      public Dumpable
    {
      Q_OBJECT;

    public:
      //
      // types
      //
      typedef std::map<Tag, SocketDispatcher*>	Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructurs & destructors
      //
      Socket();
      ~Socket();

      //
      // methods
      //
      Status		Create(const Natural16);
      Status		Create();

      Status		Send(const Location&,
			     const Packet&);

      template <const Tag G, typename T, class C>
      Status		Register(C*,
				 void (C::*)(const Location&,
					     const T&));

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      ::QUdpSocket*	socket;
      Natural16		port;

      Container		callbacks;

    private slots:
      void		Process();
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Socket.hxx>

#endif
