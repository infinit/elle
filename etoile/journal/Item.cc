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
// updated       julien quintard   [tue apr  6 11:44:56 2010]
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
    /// push constructor.
    ///
    Item::Item(hole::Address*					address):
      operation(OperationPop),
      address(address)
    {
    }

    ///
    /// default constructor.
    ///
    Item::Item(hole::Block*					block):
      operation(OperationPush),
      block(block)
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

      switch (this->operation)
	{
	case OperationPush:
	  {
	    // dump the block pointer.
	    std::cout << alignment << Dumpable::Shift << "[Block] "
		      << std::hex << this->block << std::endl;

	    break;
	  }
	case OperationPop:
	  {
	    // dump the address pointer.
	    std::cout << alignment << Dumpable::Shift << "[Address] "
		      << std::hex << this->address << std::endl;

	    break;
	  }
	}

      leave();
    }

  }
}
