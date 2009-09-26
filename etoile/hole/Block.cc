//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/hole/Block.cc
//
// created       julien quintard   [fri sep 11 22:44:58 2009]
// updated       julien quintard   [fri sep 11 22:48:20 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/hole/Block.hh>

namespace etoile
{
  namespace hole
  {

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the block's internals.
    ///
    Status		Block::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      std::cout << alignment << "[Block]" << std::endl;

      // dump the block address.
      if (this->address.Dump(margin) == StatusError)
	escape("unable to dump the address");

      leave();
    }

  }
}
