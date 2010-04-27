//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/network/Socket.hh
//
// created       julien quintard   [wed feb  3 12:49:33 2010]
// updated       julien quintard   [mon apr 26 18:31:42 2010]
//

#ifndef ELLE_NETWORK_SOCKET_HH
#define ELLE_NETWORK_SOCKET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/io/IO.hh>
#include <elle/concurrency/Concurrency.hh>

#include <elle/miscellaneous/Status.hh>
#include <elle/miscellaneous/Callback.hh>

#include <elle/network/Parcel.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace io;

  namespace network
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
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
      public Meta,
      public Dumpable
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
      Status		Monitor(const Callback<const String>&);
      Status		Withdraw();

      //
      // attributes
      //
      Type			type;
      Mode			mode;

      Container			queue;

      Callback<const String>*	callback;
    };

  }
}

#endif
