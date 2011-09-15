//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed feb  3 12:49:33 2010]
//

#ifndef ELLE_NETWORK_SOCKET_HH
#define ELLE_NETWORK_SOCKET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>

#include <elle/network/Locus.hh>
#include <elle/network/Session.hh>

#include <elle/concurrency/Event.hh>
#include <elle/concurrency/Callback.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace concurrency;

  namespace network
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// the Parcel type must be forward declare to avoid conflicts.
    ///
    class Parcel;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class abstracts the notion of socket. indeed, the socket
    /// can be non-connected i.e a slot, connected to a bridge or connected
    /// locally to a lane.
    ///
    class Socket:
      public Entity
    {
    public:
      //
      // enumerations
      //
      enum Type
	{
	  TypeUnknown,

	  TypeChannel = 0x1,

	  TypeDoor = TypeChannel | 0x0,
	  TypeGate = TypeChannel | 0x2,

	  TypeSlot = 0x4
	};

      //
      // constructors & destructors
      //
      Socket();
      Socket(const Type);

      //
      // methods
      //
      template <typename I>
      Status		Send(const I,
			     const Event& = Event::Null,
			     const Locus& = Locus::Null);
      template <typename O>
      Status		Receive(const Event&,
				O);
      template <typename I,
		typename O>
      Status		Call(const I,
			     O,
			     const Locus& = Locus::Null);
      template <typename I>
      Status		Reply(const I,
			      Session* = NULL);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Type		type;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Socket.hxx>

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Parcel.hh>

#endif
