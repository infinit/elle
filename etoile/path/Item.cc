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
// updated       julien quintard   [fri aug  7 22:53:57 2009]
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
    Item::item():
      type(TypeUnknown)
      directory(NULL)
    {
    }

    ///
    /// the destructor.
    ///
    Item::~Item()
    {
      // purge the object.
      this->Purge();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method purges the hierachy by releasing any item.
    ///
    Status		Item::Purge()
    {
      Item::Iterator	iterator;

      for (this->hierarchy != NULL)
	{
	  // purge every sub-item.
	  for (iterator = this->hierarchy.begin();
	       iterator = this->hierarchy.end();
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

    ///
    /// this method receives a route and an iterator on an element
    /// of this resolution. the method checks that this item is up-to-date
    /// and forwards the call to its hierarchy.
    ///
    Status		Item::Update(const Resolution::Explorer& explorer)
    {
      

      leave();
    }

  }
}
