//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/Holeable.cc
//
// created       julien quintard   [thu may 12 10:20:19 2011]
// updated       julien quintard   [mon jul 11 16:34:50 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Holeable.hh>

namespace hole
{

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  Holeable::Holeable(const nucleus::Network&			network):
    network(network)
  {
  }

  ///
  /// destructor.
  ///
  Holeable::~Holeable()
  {
  }

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps the implementation.
  ///
  elle::Status		Holeable::Dump(const elle::Natural32	margin) const
  {
    elle::String	alignment(margin, ' ');

    enter();

    std::cout << alignment << "[Holeable]" << std::endl;

    // dump the network.
    if (this->network.Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the network");

    leave();
  }

}
