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
// updated       julien quintard   [fri may 20 20:42:43 2011]
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
    Item::Item(const nucleus::Address&				address,
	       nucleus::Block*					block):
      operation(OperationPush),
      address(address),
      block(block)
    {
    }

    ///
    /// constructor.
    ///
    Item::Item(const nucleus::Address&				address):
      operation(OperationWipe),
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
    elle::Status	Item::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Item]" << std::endl;

      // dump the operation.
      std::cout << alignment << elle::Dumpable::Shift << "[Operation] "
		<< this->operation << std::endl;

      // dump the address.
      if (this->address.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the address");

      // display operation-specific information.
      switch (this->operation)
	{
	case OperationPush:
	  {
	    // dump the block pointer.
	    std::cout << alignment << elle::Dumpable::Shift << "[Block] "
		      << std::hex << this->block << std::endl;

	    break;
	  }
	case OperationWipe:
	  {
	    // nothing more.

	    break;
	  }
	}

      leave();
    }

  }
}
