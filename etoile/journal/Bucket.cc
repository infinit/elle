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
// updated       julien quintard   [tue apr  6 11:46:34 2010]
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
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method records a block to be pushed to the hole.
    ///
    Status		Bucket::Record(hole::Block*		block)
    {
      Item*		item;

      enter(instance(item));

      // allocate and create an item.
      item = new Item(block);

      // add the item to the container.
      this->container.push_back(item);

      // stop tracking.
      waive(item);

      leave();
    }

    ///
    /// this method records a block to be poped from the hole.
    ///
    Status		Bucket::Record(hole::Address*		address)
    {
      Item*		item;

      enter(instance(item));

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
