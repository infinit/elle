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
// updated       julien quintard   [fri dec 18 15:57:42 2009]
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
// ---------- constructs & destructors ----------------------------------------
//

    Block::Block():
      family(FamilyUnknown)
    {
    }

    Block::Block(const Family					family):
      family(family)
    {
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the block's internals.
    ///
    Status		Block::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Block]" << std::endl;

      // dump the block address.
      if (this->address.Dump(margin + 2 ) == StatusError)
	escape("unable to dump the address");

      // dump the family.
      std::cout << alignment << shift << "[Family] "
		<< this->family << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method archives the block attributes.
    ///
    Status		Block::Serialize(Archive&		archive) const
    {
      // serialize the attributes.
      if (archive.Serialize(this->address, (Byte&)this->family) == StatusError)
	escape("unable to serialize the block's attributes");

      leave();
    }

    ///
    /// this method extracts the attributes.
    ///
    Status		Block::Extract(Archive&			archive)
    {
      // extracts the attributes.
      if (archive.Extract(this->address, (Byte&)this->family) == StatusError)
	escape("unable to extract the block's attributes");
    }

  }
}
