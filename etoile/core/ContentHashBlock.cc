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
// updated       julien quintard   [mon aug 31 21:11:27 2009]
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
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns the address of the block.
    ///
    Status		ContentHashBlock::Self(hole::Address&	address)
      const
    {
      // compute the address.
      if (address.Create(*this) == StatusError)
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

      // retrieve the address of this object.
      if (this->Self(self) == StatusError)
	flee("unable to retrieve the block's address");

      address.Dump();
      self.Dump();

      // compare the address with the given one.
      if (self != address)
	flee("the given address does not correspond to this block");

      true();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		ContentHashBlock::Dump(Natural32	margin) const
    {
      String		alignment(margin, ' ');

      std::cout << alignment << "[ContentHashBlock]" << std::endl;

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
