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
// updated       julien quintard   [wed jul  6 15:02:54 2011]
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

}
