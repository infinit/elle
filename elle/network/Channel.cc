//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Channel.cc
//
// created       julien quintard   [thu mar 18 21:20:27 2010]
// updated       julien quintard   [tue mar 30 19:33:13 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Channel.hh>

namespace elle
{
  namespace network
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Channel::Channel(Socket::Type				type,
		     Socket::Mode				mode):
      Socket::Socket(type, mode)
    {
    }

  }
}
