//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/Block.cc
//
// created       julien quintard   [fri sep 11 22:44:58 2009]
// updated       julien quintard   [wed jun 22 12:36:16 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Block.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the block files extension.
  ///
  const elle::String		Block::Extension = ".blk";

//
// ---------- constructs & destructors ----------------------------------------
//

    Block::Block():
      family(FamilyUnknown),
      component(neutron::ComponentUnknown),
      _state(StateClean)
    {
    }

    Block::Block(const Family					family,
		 const neutron::Component			component):
      family(family),
      component(component),
      _state(StateClean)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method computes the address of the block.
    ///
    elle::Status	Block::Bind(Address&) const
    {
      enter();

      escape("this method should never have been called");
    }

    ///
    /// this method returns StatusTrue if the block is valid, StatusFalse
    /// otherwise.
    ///
    elle::Status	Block::Validate(const Address&) const
    {
      enter();

      escape("this method should never have been called");
    }

//
// ---------- operators -------------------------------------------------------
//

    ///
    /// this operator returns StatusTrue if the given block represents
    /// a valid update of the current block.
    ///
    elle::Boolean	Block::operator<(const Block&) const
    {
      enter();

      escape("this method should never have been called");
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Block, _());

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

      // dump the network.
      if (this->network.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the network");

      // dump the family.
      std::cout << alignment << elle::Dumpable::Shift << "[Family] "
		<< (elle::Natural32)this->family << std::endl;

      // dump the component.
      std::cout << alignment << elle::Dumpable::Shift << "[Component] "
		<< (elle::Natural32)this->component << std::endl;

      // dump the state.
      std::cout << alignment << elle::Dumpable::Shift << "[_State] "
		<< (elle::Natural32)this->_state << std::endl;

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
      if (archive.Serialize(this->network,
			    (elle::Natural8&)this->family,
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
      if (archive.Extract(this->network,
			  (elle::Natural8&)this->family,
			  (elle::Natural8&)this->component) ==
	  elle::StatusError)
	escape("unable to extract the block's attributes");

      leave();
    }

//
// ---------- fileable --------------------------------------------------------
//

    ///
    /// this method erases the block file.
    ///
    elle::Status	Block::Erase(const Network&,
				     const Address&) const
    {
      enter();

      escape("this method should never have been called");
    }

  }
}
