//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/remote/Node.cc
//
// created       julien quintard   [thu may 26 11:21:43 2011]
// updated       julien quintard   [thu may 26 11:24:19 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/remote/Node.hh>

namespace hole
{
  namespace remote
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Node::Node(const nucleus::Network&				network,
	       const elle::Address&				host):
      network(network),
      host(host)
    {
    }

  }
}
