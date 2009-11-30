//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Packet.cc
//
// created       julien quintard   [sat nov 28 13:37:14 2009]
// updated       julien quintard   [sun nov 29 20:29:29 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Packet.hh>

namespace elle
{
  namespace network
  {

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps just the packet's tag.
    ///
    Status		Packet::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Packet]" << std::endl;

      std::cout << alignment << shift << "[Tag] "
		<< this->tag << std::endl;

      leave();
    }

  }
}
