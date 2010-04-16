//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/journal/Item.cc
//
// created       julien quintard   [mon apr  5 21:26:41 2010]
// updated       julien quintard   [fri apr 16 14:33:24 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/journal/Item.hh>

namespace etoile
{
  namespace journal
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// constructor.
    ///
    Item::Item(const hole::Address&				address,
	       hole::Block*					block):
      operation(OperationPush),
      address(address),
      block(block)
    {
    }

    ///
    /// constructor.
    ///
    Item::Item(const hole::Address&				address):
      operation(OperationDestroy),
      address(address),
      block(NULL)
    {
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the item.
    ///
    Status		Item::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Item]" << std::endl;

      // dump the operation.
      std::cout << alignment << Dumpable::Shift << "[Operation] "
		<< this->operation << std::endl;

      // dump the address.
      if (this->address.Dump(margin + 2) == StatusError)
	escape("unable to dump the address");

      // display operation-specific information.
      switch (this->operation)
	{
	case OperationPush:
	  {
	    // dump the block pointer.
	    std::cout << alignment << Dumpable::Shift << "[Block] "
		      << std::hex << this->block << std::endl;

	    break;
	  }
	case OperationDestroy:
	  {
	    // nothing more.

	    break;
	  }
	}

      leave();
    }

  }
}
