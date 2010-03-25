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
// updated       julien quintard   [wed mar 24 13:14:43 2010]
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
    Channel::Channel(Socket::Type				type):
      Socket::Socket(type)
    {
    }

  }
}
