//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Environment.cc
//
// created       julien quintard   [thu feb  4 01:01:29 2010]
// updated       julien quintard   [thu feb  4 01:03:02 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Environment.hh>

namespace elle
{
  namespace network
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// specific constructor.
    ///
    Environment::Environment(Socket&				socket):
      socket(socket)
    {
    }

  }
}
