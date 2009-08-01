//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/cache/Cache.cc
//
// created       julien quintard   [fri jul 31 14:59:28 2009]
// updated       julien quintard   [sat aug  1 23:35:41 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/cache/Cache.hh>

namespace etoile
{
  namespace cache
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method looks for a stored item matching the given address
    /// and returns true or false accordingly.
    ///
    Status		Cache::Search(String&			identity,
				      Cache::Iterator*		iterator)
    {
      Cache::Iterator	i;

      // look for the string in the container.
      if ((i = Cache::container.find(identity)) == Cache::container.end())
	false();

      // set the iterator if required.
      if (iterator != NULL)
	*iterator = i;

      true();
    }

    ///
    /// this method insert an item.
    ///
    Status		Cache::Put(String&			identity,
				   Block*			block)
    {
      /*
      Cache::Iterator	iterator;

      // if there is already an item, release it.
      if (Cache::Search(address, iterator) == StatusTrue)
	{
	  Block*	value = *iterator;

	  // release the stored value.
	  delete value;

	  // insert the item.
	  *iterator = block;
	}
      */
      leave();
    }

    ///
    /// this method returns an item or error if there is no such item.
    ///
    Status		Cache::Get(String&			identity,
				   Block*			block)
    {
      /*
      Cache::Iterator	iterator;

      // if there is already an item, release it.
      if (Cache::Search(address, iterator) == StatusFalse)
	escape("XXX");
      */

      leave();
    }

    Status		Cache::Erase(String&			identity)
    {
      // XXX

      leave();
    }

  }
}
