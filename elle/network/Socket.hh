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
// updated       julien quintard   [tue jun  7 06:14:58 2011]
//

#ifndef ELLE_NETWORK_SOCKET_HH
#define ELLE_NETWORK_SOCKET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>

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

      enum Mode
	{
	  ModeUnknown,

	  ModeSynchronous,
	  ModeAsynchronous
	};

      //
      // types
      //
      typedef std::list<Parcel*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructors & destructors
      //
      Socket();
      Socket(const Type,
	     const Mode);
      ~Socket();

      //
      // methods
      //
      Status		Monitor(const Callback< Parameters<const String> >&);
      Status		Withdraw();

      //
      // attributes
      //
      Type					type;
      Mode					mode;

      Container					queue;

      Callback< Parameters<const String> >*	callback;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Parcel.hh>

#endif
