//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/cache/Cache.cc
//
// created       julien quintard   [sat aug  1 01:15:37 2009]
// updated       julien quintard   [sat aug  1 23:37:45 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/cache/Cache.hh>

namespace pig
{
  namespace cache
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// XXX
    ///
    Cache::Container	Cache::container;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Cache::Search(const String&		path,
				      Address*			address)
    {
      Cache::Iterator	iterator;

      // look for the string in the container.
      if ((iterator = Cache::container.find(path)) == Cache::container.end())
	false();

      // set the iterator if required.
      if (address != NULL)
	{
	  Cache::Item*	item = *iterator;

	  *address = item->address;
	}

      true();
    }

    ///
    /// XXX
    ///
    Status		Cache::Put(const String&		path,
				   const Address&		address,
				   const Cache::Type&		type)
    {
      Cache::Item*	item;

      // allocate a new item.
      item = new Cache::Item;

      // create the item.
      item->type = type;
      item->address = address;

      // insert the item.
      Cache::container.insert(std::pair<String, Cache::Item*>(path, item));

      leave();
    }

    ///
    /// XXX
    ///
    Status		Cache::Get(const String&		path,
				   Address&			address)
    {
      Cache::Item*	item;

      item = container[path];

      address = item->address;

      leave();
    }

  }
}
