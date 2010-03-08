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
// updated       julien quintard   [mon mar  8 23:07:57 2010]
//

#ifndef ELLE_NETWORK_SLOT_HH
#define ELLE_NETWORK_SLOT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/network/Socket.hh>
#include <elle/network/Port.hh>
#include <elle/network/Address.hh>
#include <elle/network/Message.hh>
#include <elle/network/Network.hh>
#include <elle/network/Data.hh>
#include <elle/network/Raw.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Identifier.hh>

#undef release
#include <QObject>
#include <QUdpSocket>
#include <elle/core/Macro.hh>

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
      template <typename O>
      Status		Receive(const Identifier&,
				O&);
      template <typename I,
		typename O>
      Status		Request(const Address&,
				const I&,
				O&);
      template <typename I,
		typename O>
      Status		Call(const Address&,
			     const I&,
			     O&);
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
      ::QUdpSocket*	socket;
      Port		port;

    private slots:
      //
      // slots
      //
      void		Fetch();
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Slot.hxx>

#endif

