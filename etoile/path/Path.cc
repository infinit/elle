//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/path/Path.cc
//
// created       julien quintard   [sat aug  8 16:21:09 2009]
// updated       julien quintard   [wed mar 17 17:44:58 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Path.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable contains the address of the root directory object.
    ///
    hole::Address	Path::Root;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initialises the path component.
    ///
    Status		Path::Initialize(const hole::Address&	address)
    {
      enter();

      // initialize the root address.
      Path::Root = address;

      // initialize the cache.
      if (Cache::Initialize() == StatusError)
	escape("unable to initialize the cache");

      leave();
    }

    ///
    /// this method cleans the path module.
    ///
    Status		Path::Clean()
    {
      enter();

      // clean the cache.
      if (Cache::Clean() == StatusError)
	escape("unable to clean the cache");

      leave();
    }

    ///
    /// this method takes a route and returns the address of the
    /// referenced object.
    ///
    /// this method starts by resolving the route by looking up in the
    /// cache. then, the resolving process retrieved the uncached directory
    /// objects and explore them.
    ///
    Status		Path::Resolve(const Route&		route,
				      hole::Address&		address)
    {
      Venue		venue;
      Route::Scoutor	scoutor;

      enter();

      // first ask the cache to resolve as much as it can.
      if (Cache::Resolve(route, venue) == StatusError)
	escape("unable to resolve part of the route through the cache");

      // if the cache did not resolve anything.
      if (venue == Venue::Null)
	{
	  // start with the root directory.
	  address = Path::Root;
	}
      else
	{
	  // set the address with the address of the last resolved element.
	  address = venue.elements[venue.elements.size() - 1];
	}

      // if complete, return the address.
      if (route.elements.size() == venue.elements.size())
	leave();

      // otherwise, resolve manually by retrieving the directory object.
      for (scoutor = route.elements.begin() + venue.elements.size();
	   scoutor != route.elements.end();
	   scoutor++)
	{
	  context::Directory	context;

	  // load the directory referenced by address.
	  if (components::Directory::Load(&context, address) == StatusError)
	    escape("unable to load one of the route's directories");

	  // look up for the name.
	  if (components::Directory::Lookup(&context,
					    *scoutor,
					    address) == StatusError)
	    escape("unable to find one of the route's entries");

	  // close the context.
	  if (components::Directory::Close(&context) == StatusError)
	    escape("unable to close the context");
	}

      // update the resolved path to the cache.
      if (Cache::Update(route, venue) == StatusError)
	escape("unable to update the cache");

      // return the target address.
      address = venue.elements[venue.elements.size() - 1];

      leave();
    }

  }
}
