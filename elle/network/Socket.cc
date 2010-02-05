//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Socket.cc
//
// created       julien quintard   [wed feb  3 12:55:47 2010]
// updated       julien quintard   [wed feb  3 12:58:55 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Socket.hh>

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
    Socket::Socket(const Socket::Type&				type):
      type(type)
    {
    }

//
// ---------- dumpable --------------------------------------------------------
//

    Status		Socket::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Socket]" << std::endl;

      // dump the type.
      std::cout << alignment << shift << "[Type] "
		<< this->type << std::endl;

      leave();
    }

  }
}
