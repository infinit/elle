//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Link.cc
//
// created       julien quintard   [thu mar 18 21:20:27 2010]
// updated       julien quintard   [thu mar 18 23:31:04 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Link.hh>

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
    Link::Link(Socket::Type					type):
      Socket::Socket(type)
    {
    }

  }
}
