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
// updated       julien quintard   [sun aug  9 00:37:42 2009]
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
    /// this variable defines the path separator.
    ///
    String		Path::Separator("/");

    ///
    /// this variable contains the address of the root directory object.
    ///
    Address		Path::Root;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initialises the path component.
    ///
    Status		Path::Initialize(const Address&		address)
    {
      // initialize the root address.
      Path::Root = address;

      leave();
    }

    ///
    /// this method cleans the path module.
    ///
    Status		Path::Clean()
    {
      // purge the cache.
      if (Cache::Purge() == StatusError)
	escape("unable to purge the cache");

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
				      Address&			address)
    {
      Venue		venue;

      // first ask the cache to resolve as much as it can.
      if (Cache::Resolve(route, venue) == StatusError)
	escape("unable to resolve part of the route through the cache");

      // if the cache did not resolve anything.
      if (venue.elements.size() == 0)
	{
	  address = Path::Root;
	}
      else
	{
	  // otherwise, start the the last resolved element.
	  //XXX
	  address = venue.elements[venue.elements.size() - 1];
	}

      leave();
    }

  }
}
