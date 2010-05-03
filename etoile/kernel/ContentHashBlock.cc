//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/ContentHashBlock.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [mon may  3 16:26:39 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/ContentHashBlock.hh>

#include <etoile/hole/Family.hh>
#include <etoile/hole/Component.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this constructor takes a family and kind and set the underlying
    /// block properties.
    ///
    ContentHashBlock::ContentHashBlock(const hole::Component&	component):
      hole::Block(hole::FamilyContentHashBlock, component)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method computes the address of the block.
    ///
    elle::Status	ContentHashBlock::Bind()
    {
      enter();

      // compute the address.
      if (this->address.Create(this->family, *this) == elle::StatusError)
	escape("unable to compute the CHB's address");

      leave();
    }

    ///
    /// this method verifies that the block is valid according to the
    /// given requested address.
    ///
    elle::Status	ContentHashBlock::Validate(const
						     hole::Address& address)
      const
    {
      hole::Address	self;

      enter();

      // compute the address of this object.
      if (self.Create(this->family, *this) == elle::StatusError)
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
    elle::Status	ContentHashBlock::Dump(const
					         elle::Natural32 margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[ContentHashBlock]" << std::endl;

      // dump the parent class.
      if (hole::Block::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the underlying block");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    elle::Status	ContentHashBlock::Serialize(elle::Archive& archive)
      const
    {
      enter();

      // serialize the parent class.
      if (hole::Block::Serialize(archive) == elle::StatusError)
	escape("unable to serialize the underlying block");

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    elle::Status	ContentHashBlock::Extract(elle::Archive& archive)
    {
      enter();

      // extract the parent class.
      if (hole::Block::Extract(archive) == elle::StatusError)
	escape("unable to extract the underlying block");

      leave();
    }

  }
}
