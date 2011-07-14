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
// updated       julien quintard   [mon jul 11 16:39:40 2011]
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

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the node.
      ///
      elle::Status	Node::Dump(const elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Node]" << std::endl;

	// dump the network.
	if (this->network.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the network");

	// dump the host.
	if (this->host.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the host");

	leave();
      }

    }
  }
}
