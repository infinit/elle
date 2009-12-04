//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/ContentHashBlock.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [tue dec  1 02:55:39 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/ContentHashBlock.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this constructor takes a family and kind and set the underlying
    /// block properties.
    ///
    ContentHashBlock::ContentHashBlock(const hole::Block::Kind	kind):
      hole::Block(hole::Block::FamilyContentHashBlock, kind)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method computes the address of the block.
    ///
    Status		ContentHashBlock::Bind()
    {
      // compute the address.
      if (this->address.Create(*this) == StatusError)
	escape("unable to compute the CHB's address");

      leave();
    }

    ///
    /// this method verifies that the block is valid according to the
    /// given requested address.
    ///
    Status		ContentHashBlock::Validate(const
						     hole::Address& address)
      const
    {
      hole::Address	self;

      // verify the recorded address with the given one.
      if (this->address != address)
	flee("the given address does not correspond to the recorded address");

      // compute the address of this object.
      if (self.Create(*this) == StatusError)
	escape("unable to compute the CHB's address");

      // compare the address with the given one.
      if (this->address != self)
	flee("the recorded address does not correspond to this block");

      true();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		ContentHashBlock::Dump(const Natural32	margin) const
    {
      String		alignment(margin, ' ');

      std::cout << alignment << "[ContentHashBlock]" << std::endl;

      // dump the parent class.
      if (hole::Block::Dump(margin + 2) == StatusError)
	escape("unable to dump the underlying block");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    Status		ContentHashBlock::Serialize(Archive&) const
    {
      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		ContentHashBlock::Extract(Archive&)
    {
      leave();
    }

  }
}
