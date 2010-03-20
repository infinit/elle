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
// updated       julien quintard   [fri mar 19 17:07:04 2010]
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
    /// this container represents the root of the hierarchical data container.
    ///
    Item			Cache::Data;

    ///
    /// this container is used for keeping track of the least recently
    /// used items.
    ///
    Cache::Container		Cache::Queue;

    ///
    /// this constant defines the capacity of the cache.
    ///
    /// whenever this threshold is reached, an item is dismissed from
    /// the cache.
    ///
    Natural32&			Cache::Capacity =
      Configuration::Path::Capacity;

    ///
    /// this variable holds the number of items currently cached.
    ///
    Natural32			Cache::Size = 0;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Cache::Initialize()
    {
      enter();

      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    Status		Cache::Clean()
    {
      enter();

      /* XXX
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
      Route::Scoutor	r;
      Venue::Scoutor	v;
      Item*		item = &Cache::Data;

      enter();

      // for every element of the route.
      for (r = route.elements.begin(), v = venue.elements.begin();
	   (r != route.elements.end()) && (v != venue.elements.end());
	   r++, v++)
	{
	  hole::Address	address;

	  // update the item with the new address.
	  if (item->Update(*r, *v) == StatusError)
	    escape("unable to update the item");

	  // try to resolve within this item.
	  if (item->Resolve(*r, item) != StatusTrue)
	    escape("unexpected resolution failure");
	}

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
      Item*		item = &Cache::Data;

      enter();

      // for every element of the route.
      for (scoutor = route.elements.begin();
	   scoutor != route.elements.end();
	   scoutor++)
	{
	  // try to resolve within this item.
	  if (item->Resolve(*scoutor, item) != StatusTrue)
	    break;

	  // add the address to the venue.
	  venue.elements.push_back(item->address);
	}

      leave();
    }

    ///
    /// this method dumps the whole cache.
    ///
    Status		Cache::Show(const Natural32		margin)
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Cache]" << std::endl;

      // just initiate a recursive dump, starting with the root item.
      if (Cache::Data.Dump(margin + 2) == StatusError)
	escape("unable to dump the cache data");

      leave();
    }

  }
}
