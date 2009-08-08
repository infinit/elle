//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/path/Item.cc
//
// created       julien quintard   [fri aug  7 21:44:49 2009]
// updated       julien quintard   [sat aug  8 21:30:07 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Item.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor.
    ///
    Item::Item():
      directory(NULL)
    {
    }

    ///
    /// the destructor.
    ///
    Item::~Item()
    {
      // XXX
      // purge the object.
      //this->Purge();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method purges the hierachy by releasing any item.
    ///
    /*
    Status		Item::Purge()
    {
      Item::Iterator	iterator;

      if (this->hierarchy != NULL)
	{
	  // purge every sub-item.
	  for (iterator = this->hierarchy->begin();
	       iterator != this->hierarchy->end();
	       iterator++)
	    {
	      Item*	item = iterator->second;

	      // purge the sub-item.
	      if (item->Purge() == StatusError)
		escape("unable to purge the sub-item");

	      // delete the item object.
	      delete item;
	    }

	  // finally delete the hiearchy container.
	  delete this->hierarchy;
	}

      leave();
    }
    */

  }
}
