//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/journal/Bucket.cc
//
// created       julien quintard   [mon apr  5 21:33:25 2010]
// updated       julien quintard   [fri apr 16 14:29:00 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/journal/Bucket.hh>

namespace etoile
{
  namespace journal
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// destructor.
    ///
    Bucket::~Bucket()
    {
      Bucket::Scoutor	scoutor;

      // delete every item.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Item*		item = *scoutor;

	  // delete the item.
	  delete item;
	}
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method records a block to be pushed to the hole.
    ///
    Status		Bucket::Push(const hole::Address&	address,
				     hole::Block*		block)
    {
      Item*		item;

      enter(instance(item));

      // check the address.
      if (address == hole::Address::Null)
	escape("a block cannot be pushed with a null address");

      // allocate and create an item.
      item = new Item(address, block);

      // add the item to the container.
      this->container.push_back(item);

      // stop tracking.
      waive(item);

      leave();
    }

    ///
    /// this method records a block to be poped from the hole.
    ///
    Status		Bucket::Destroy(const hole::Address&	address)
    {
      Item*		item;

      enter(instance(item));

      // check the address.
      if (address == hole::Address::Null)
	escape("a null-address-referenced block cannot be destroyed");

      // allocate and create an item.
      item = new Item(address);

      // add the item to the container.
      this->container.push_back(item);

      // stop tracking.
      waive(item);

      leave();
    }

  }
}
