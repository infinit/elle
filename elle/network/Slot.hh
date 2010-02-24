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
// updated       julien quintard   [wed feb 24 11:14:14 2010]
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
#include <elle/network/Environment.hh>
#include <elle/network/Network.hh>
#include <elle/network/Data.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Inputs.hh>

#include <QObject>
#include <QUdpSocket>

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
			     const I&);
      template <typename I>
      Status		Transmit(const Address&,
				 const I&);
      template <typename O>
      Status		Receive(Address&,
				O&);
      template <typename I,
		typename O>
      Status		Request(const Address&,
				const I&,
				O&);
      // Request: asynchrone
      // Call: synchrone

      template <typename I>
      Status		Reply(const Address&,
			      const I&);

      // XXX
      // Send(): asynchronous i.e send and return.
      // Transmit(): synchronous i.e send and wait for a ACK (unless in TCP)
      // Receive():
      // XXX

      //
      // attributes
      //
      ::QUdpSocket*	socket;
      Port		port;

    private slots:
      //
      // slots
      //
      void		Read();
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Slot.hxx>

#endif

