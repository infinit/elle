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
// updated       julien quintard   [wed mar  3 16:12:11 2010]
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
    ContentHashBlock::ContentHashBlock():
      hole::Block(hole::FamilyContentHashBlock)
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
      enter();

      // compute the address.
      if (this->address.Create(this->family, *this) == StatusError)
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

      enter();

      // compute the address of this object.
      if (self.Create(this->family, *this) == StatusError)
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

      enter();

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
    Status		ContentHashBlock::Serialize(Archive&	archive) const
    {
      enter();

      // serialize the parent class.
      if (hole::Block::Serialize(archive) == StatusError)
	escape("unable to serialize the underlying block");

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		ContentHashBlock::Extract(Archive&	archive)
    {
      enter();

      // extract the parent class.
      if (hole::Block::Extract(archive) == StatusError)
	escape("unable to extract the underlying block");

      leave();
    }

  }
}
