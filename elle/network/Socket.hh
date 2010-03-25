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
// updated       julien quintard   [thu mar 25 01:44:05 2010]
//

#ifndef ELLE_NETWORK_SOCKET_HH
#define ELLE_NETWORK_SOCKET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/io/IO.hh>
#include <elle/concurrency/Concurrency.hh>

#include <elle/misc/Status.hh>
#include <elle/misc/Callback.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace io;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//


    ///
    /// this class abstracts the notion of socket. indeed, the socket
    /// can be non-connected i.e a slot, connected to a channel or connected
    /// locally to a bridge.
    ///
    class Socket:
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

      //
      // constructors & destructors
      //
      Socket();
      Socket(const Type&);
      ~Socket();

      //
      // methods
      //
      Status		Monitor(Callback&);
      Status		Withdraw();

      //
      // attributes
      //
      Type		type;

      Callback*		callback;
    };

  }
}

#endif
