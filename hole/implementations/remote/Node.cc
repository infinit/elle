//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Node.cc
//
// created       julien quintard   [thu may 26 11:21:43 2011]
// updated       julien quintard   [tue jul  5 17:42:33 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Node.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// default constructor.
      ///
      Node::Node(const nucleus::Network&			network,
		 const elle::Address&				host):
	network(network),
	host(host)
      {
      }

    }
  }
}
