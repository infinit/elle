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
// updated       julien quintard   [wed mar 31 21:33:17 2010]
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

      enter();

      std::cout << alignment << "[Block]" << std::endl;

      // dump the block address.
      if (this->address.Dump(margin + 2 ) == StatusError)
	escape("unable to dump the address");

      // dump the family.
      std::cout << alignment << Dumpable::Shift << "[Family] "
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
      enter();

      // serialize the attributes.
      if (archive.Serialize((Natural8&)this->family) == StatusError)
	escape("unable to serialize the block's attributes");

      leave();
    }

    ///
    /// this method extracts the attributes.
    ///
    Status		Block::Extract(Archive&			archive)
    {
      enter();

      // extracts the attributes.
      if (archive.Extract((Natural8&)this->family) == StatusError)
	escape("unable to extract the block's attributes");

      leave();
    }

  }
}
