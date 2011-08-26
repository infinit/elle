//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Peer.cc
//
// created       julien quintard   [thu may 26 11:21:43 2011]
// updated       julien quintard   [wed aug 24 10:00:04 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Peer.hh>

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
      Peer::Peer(const nucleus::Network&			network):
	network(network)
      {
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the peer.
      ///
      elle::Status	Peer::Dump(const elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Peer]" << std::endl;

	// dump the network.
	if (this->network.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the network");

	leave();
      }

    }
  }
}
