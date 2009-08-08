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
// updated       julien quintard   [sat aug  8 01:49:52 2009]
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
    Natural32		Cache::Capacity = 4096;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the paths cache.
    ///
    Status		Cache::Initialize(Address&		address)
    {
      // initialize the root directory item.
      this->type = Item::TypeDirectory;
      this->root.address = address;

      leave();
    }

    ///
    /// this method cleans the cache by purging it.
    ///
    Status		Cache::Clean()
    {
      // purge the cache.
      if (this->Purge() == StatusError)
	escape("unable to purge the cache");

      leave();
    }

    ///
    /// this method purges the cache by releasing all the cached items.
    ///
    /// note that this method just calls the Purge() method on the
    /// root item because every node cleans its hierarchy by itself.
    ///
    Status		Cache::Purge()
    {
      // purge the hierarchy.
      if (this->root.Purge() == StatusError)
	escape("unable to purge the root directory item");

      leave();
    }

    ///
    /// this method adds a path to the cache by specifying
    ///
    Status		Cache::Update(const Path&		path,
				      const Route&		route)
    {
      // update the path.
      if (this->root.hierarchy != NULL)
	{
	  Path::Explorer	i = path.begin();
	  Route::Explorer	j = route.begin();
	  Item::Iterator	k;
	  Item*			item;

	  // look up for the name.
	  if ((k = this->root.hiearchy.find(*i)) == this->root.hierarchy.end())
	    {
	      // create the entry.
	      if (this->root.Create(*i, *j) == StatusError)
		escape("unable to create an entry");
	    }
	  else
	    {
	      // update the entry.
	      if (this->root.Update(i + 1, j + 1) == StatusError)
		escape("unable to update the entry");
	    }

      leave();
    }

    ///
    /// this method takes a path in the form of a list of names and
    /// returns both the address of the largest cached item plus an
    /// updated list of the names remaining to be resolved manually.
    ///
    Status		Cache::Resolve(const Path&		path,
				       Resolution&		resolution)
    {
      // XXX

      leave();
    }

  }
}
