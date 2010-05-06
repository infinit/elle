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
// updated       julien quintard   [tue may  4 10:21:37 2010]
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
      family(FamilyUnknown),
      component(ComponentUnknown)
    {
    }

    Block::Block(const Family&					family,
		 const Component&				component):
      family(family),
      component(component)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method computes the address of the block.
    ///
    elle::Status	Block::Bind()
    {
      enter();

      fail("this method should never have been called");
    }

    ///
    /// this method returns StatusTrue if the block is valid, StatusFalse
    /// otherwise.
    ///
    elle::Status	Block::Validate(const Address&) const
    {
      enter();

      fail("this method should never have been called");
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Block, _(), _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the block's internals.
    ///
    elle::Status	Block::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Block]" << std::endl;

      // dump the block address.
      if (this->address.Dump(margin + 2 ) == elle::StatusError)
	escape("unable to dump the address");

      // dump the family.
      std::cout << alignment << elle::Dumpable::Shift << "[Family] "
		<< (elle::Natural32)this->family << std::endl;

      // dump the component.
      std::cout << alignment << elle::Dumpable::Shift << "[Component] "
		<< (elle::Natural32)this->component << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method archives the block attributes.
    ///
    elle::Status	Block::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize((elle::Natural8&)this->family,
			    (elle::Natural8&)this->component) ==
	  elle::StatusError)
	escape("unable to serialize the block's attributes");

      leave();
    }

    ///
    /// this method extracts the attributes.
    ///
    elle::Status	Block::Extract(elle::Archive&		archive)
    {
      enter();

      // extracts the attributes.
      if (archive.Extract((elle::Natural8&)this->family,
			  (elle::Natural8&)this->component) ==
	  elle::StatusError)
	escape("unable to extract the block's attributes");

      leave();
    }

  }
}
