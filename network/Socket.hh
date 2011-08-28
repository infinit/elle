//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Socket.hh
//
// created       julien quintard   [wed feb  3 12:49:33 2010]
// updated       julien quintard   [sat aug 27 00:42:01 2011]
//

#ifndef ELLE_NETWORK_SOCKET_HH
#define ELLE_NETWORK_SOCKET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>

#include <elle/network/Point.hh>
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
    /// can be non-connected i.e a slot, connected to a channel or connected
    /// locally to a bridge.
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
      ~Socket();

      //
      // methods
      //
      template <typename I>
      Status		Send(const I,
			     const Event& = Event::Null,
			     const Point& = Point::Null);
      template <typename O>
      Status		Receive(const Event&,
				O);
      template <typename I,
		typename O>
      Status		Call(const I,
			     O,
			     const Point& = Point::Null);
      template <typename I>
      Status		Reply(const I,
			      Session* = NULL);

      Status		Monitor(const Callback< Status,
						Parameters<> >&);
      Status		Withdraw();

      //
      // callbacks
      //
      Status		Signal();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Type		type;

      Callback<
	Status,
	Parameters<>
	>*		callback;
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
