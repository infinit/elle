//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/ContentHashBlock.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [thu may  5 13:41:27 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/ContentHashBlock.hh>
#include <nucleus/proton/Family.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    ContentHashBlock::ContentHashBlock():
      Block(FamilyContentHashBlock)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method computes the address of the block.
    ///
    elle::Status	ContentHashBlock::Bind(Address&		address)
      const
    {
      enter();

      // compute the address.
      if (address.Create(*this) == elle::StatusError)
	escape("unable to compute the CHB's address");

      leave();
    }

    ///
    /// this method verifies that the block is valid according to the
    /// given requested address.
    ///
    elle::Status	ContentHashBlock::Validate(const Address& address)
      const
    {
      Address		self;

      enter();

      // compute the address of this object.
      if (self.Create(*this) == elle::StatusError)
	escape("unable to compute the CHB's address");

      // compare the address with the given one.
      if (address != self)
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
      if (Block::Dump(margin + 2) == elle::StatusError)
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
      if (Block::Serialize(archive) == elle::StatusError)
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
      if (Block::Extract(archive) == elle::StatusError)
	escape("unable to extract the underlying block");

      // check the family.
      if (this->family != FamilyContentHashBlock)
	escape("invalid family");

      leave();
    }

  }
}
