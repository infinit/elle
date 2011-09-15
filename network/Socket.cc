//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed feb  3 12:55:47 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Socket.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace network
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor
    ///
    Socket::Socket():
      type(Socket::TypeUnknown)
    {
    }

    ///
    /// a constructor which specifies the type of socket.
    ///
    Socket::Socket(const Socket::Type				type):
      type(type)
    {
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the socket-related information.
    ///
    Status		Socket::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Socket] " << std::hex << this << std::endl;

      // dump the type.
      std::cout << alignment << Dumpable::Shift
		<< "[Type] " << std::dec << this->type << std::endl;

      leave();
    }

  }
}
