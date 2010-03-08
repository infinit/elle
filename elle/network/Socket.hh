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
// updated       julien quintard   [fri mar  5 16:02:41 2010]
//

#ifndef ELLE_NETWORK_SOCKET_HH
#define ELLE_NETWORK_SOCKET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/io/IO.hh>

#include <elle/network/Message.hh>

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
	  TypeDoor,
	  TypeSlot,
	  TypeGate
	};

      //
      // constructors & destructors
      //
      Socket();
      Socket(const Type&);

      //
      // attributes
      //
      Type	type;
    };

  }
}

#endif
