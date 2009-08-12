//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/path/Cache.cc
//
// created       julien quintard   [fri aug  7 20:51:38 2009]
// updated       julien quintard   [mon aug 10 20:52:24 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Cache.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant defines the capacity of the cache.
    ///
    /// whenever this threshold is reached, an item is dismissed from
    /// the cache.
    ///
    const Natural32		Cache::Capacity = 4096;

    ///
    /// this container is used for keeping track of the least recently
    /// used items.
    Cache::Container		Cache::Queue;

    ///
    /// this variable holds the number of items currently cached.
    ///
    Natural32			Cache::Size = 0;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method purges the cache by releasing all the cached items.
    ///
    /// note that this method just calls the Purge() method on the
    /// root item because every node cleans its hierarchy by itself.
    ///
    Status		Cache::Purge()
    {
      /*
      // purge the hierarchy.
      if (Cache::Root.Purge() == StatusError)
	escape("unable to purge the root directory item");
      */
      leave();
    }

    ///
    /// this method adds/updates a path.
    ///
    Status		Cache::Update(const Route&		route,
				      const Venue&		venue)
    {
      // update the path.
      //if (Cache::Root.hierarchy != NULL)
      //{
	  /*
	  Route::Scoutor	r = route.elements.begin();
	  Venue::Scoutor	v = venue.elements.begin();
	  Item::Iterator	i;
	  Item*			item;

	  // look up for the name in the root item.
	  if ((i = Cache::Root.hierarchy->find(*r)) ==
	      Cache::Root.hierarchy->end())
	    {
	      // create the entry.
	      if (Cache::Root.Create(*r, *v, &item) == StatusError)
		escape("unable to create an entry");
	    }
	  else
	    {
	      /// \todo XXX note that we test here before we update because
	      //    in a threaded environment, it would take use to acquire
	      //    a write lock to update.

	      // retrieve the item.
	      item = i->second;

	      // otherwise, if the address is different, update it.
	      if (item->address != *v)
		item->address = *v;
	    }

	  // update the sub-item.
	  //if (item->Update(r + 1, v + 1) == StatusError)
	  //escape("unable to update the sub-item");
	  */
      //}

      leave();
    }

    ///
    /// this method takes a path in the form of a list of names and
    /// returns both the address of the largest cached item plus an
    /// updated list of the names remaining to be resolved manually.
    ///
    Status		Cache::Resolve(const Route&		route,
				       Venue&			venue)
    {
      Route::Scoutor	scoutor;
      Item::Iterator	iterator;
      Item*		item;
      /*
      // for every element of the route.
      for (scoutor = route.elements.begin();
	   scoutor != route.elements.end();
	   scoutor++)
	{
	  // stop if there is no more entries.
	  if (item->hierarchy == NULL)
	    break;

	  // look up the element in the current item .. stop if not present.
	  if ((iterator = item->hierarchy->find(*scoutor)) ==
	      item->hierarchy->end())
	    break;

	  // add the address to the venue.
	  venue.elements.push_back(iterator->second->address);
	}
      */
      leave();
    }

  }
}
