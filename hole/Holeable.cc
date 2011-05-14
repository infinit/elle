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
// updated       julien quintard   [thu may 12 10:51:18 2011]
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
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method sets the address of the root directory for this network.
  ///
  elle::Status		Holeable::Root(const nucleus::Address&	address)
  {
    enter();

    // set the root address.
    this->root = address;

    leave();
  }

}
