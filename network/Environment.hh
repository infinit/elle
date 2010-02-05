//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Environment.hh
//
// created       julien quintard   [wed feb  3 16:14:24 2010]
// updated       julien quintard   [thu feb  4 01:00:03 2010]
//

#ifndef ELLE_NETWORK_ENVIRONMENT_HH
#define ELLE_NETWORK_ENVIRONMENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Address.hh>
#include <elle/network/Socket.hh>

namespace elle
{
  namespace network
  {

//
// ---------- structures ------------------------------------------------------
//

    ///
    /// XXX
    ///
    struct Environment
    {
    public:
      //
      // constructors & destructors
      //
      Environment(Socket&);

      //
      // attributes
      //
      Address		address;
      Socket&		socket;
    };

  }
}

#endif

