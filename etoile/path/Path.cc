//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/path/Path.cc
//
// created       julien quintard   [sat aug  8 16:21:09 2009]
// updated       julien quintard   [sat jun 25 16:30:23 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Path.hh>

#include <etoile/gear/Directory.hh>

#include <etoile/automaton/Directory.hh>

#include <etoile/depot/Depot.hh>

#include <nucleus/Nucleus.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initialises the path component.
    ///
    elle::Status	Path::Initialize()
    {
      enter();

      /* XXX
      // initialize the cache.
      if (Cache::Initialize() == elle::StatusError)
	escape("unable to initialize the cache");
      */

      leave();
    }

    ///
    /// this method cleans the path module.
    ///
    elle::Status	Path::Clean()
    {
      enter();

      /* XXX
      // clean the cache.
      if (Cache::Clean() == elle::StatusError)
	escape("unable to clean the cache");
      */

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
    elle::Status	Path::Resolve(const Route&		route,
				      Venue&			venue)
    {
      nucleus::Address	address;
      nucleus::Version	version;
      Route::Scoutor	scoutor;

      enter();

      // first ask the cache to resolve as much as it can.
      //if (Cache::Resolve(route, venue) == elle::StatusError)
      //escape("unable to resolve part of the route through the cache");

      // if complete, return the address i.e without updating the cache.
      if (route.elements.size() == venue.elements.size())
	leave();

      // if the cache did not resolve anything.
      if (venue == Venue::Null)
	{
	  Slice		slice;

	  // retrieve the root directory's address.
	  if (depot::Depot::Origin(address) == elle::StatusError)
	    escape("unable to retrieve the address of the root directory");

	  // parse the very first slab i.e the root slab in order
	  // to extract the version number. note that the root slab is
	  // always empty.
	  if (Path::Parse(route.elements[0],
			  slice, version) == elle::StatusError)
	    escape("unable to extract the version number from the root slab");

	  // record the root directory in the venue.
	  if (venue.Record(address, version) == elle::StatusError)
	    escape("unable to record the root directory in the venue");
	}

      // set the address/version with the address of the last resolved element.
      address = venue.elements[venue.elements.size() - 1].address;
      version = venue.elements[venue.elements.size() - 1].version;

      // otherwise, resolve manually by retrieving the directory object.
      for (scoutor = route.elements.begin() + venue.elements.size();
	   scoutor != route.elements.end();
	   scoutor++)
	{
	  //
	  // note here that all operations are performed on a local scope.
	  //
	  // this scope is not exported because no application needs to
	  // access it. therefore it is not allocated nor added to the gear
	  // container.
	  //

	  gear::Directory	context;
	  Slice			slice;
	  nucleus::Entry	entry;
	  nucleus::Location	location;

	  // create the location.
	  if (location.Create(address, version) == elle::StatusError)
	    escape("unable to create the location");

	  // fetch the directory.
	  if (automaton::Directory::Load(context,
					 location) == elle::StatusError)
	    escape("unable to fetch the directory object");

	  // extract the slice/version from the current slab.
	  if (Path::Parse(*scoutor,
			  slice,
			  version) == elle::StatusError)
	    escape("unable to extract the slice/version from the "
		   "current slab");

	  // look up for the name.
	  if (automaton::Directory::Lookup(context,
					   slice,
					   entry) == elle::StatusError)
	    escape("unable to find one of the route's entries");

	  // if there is no such entry, abort.
	  if (entry == nucleus::Entry::Null)
	    escape("unable to locate the directory entry");

	  // set the address; the version is already set i.e it has
	  // been extracted from the slab.
	  address = entry.address;

	  // first, record the address/version in the venue.
	  if (venue.Record(address, version) == elle::StatusError)
	    escape("unable to record the venue address");
	}

      // update the resolved path to the cache.
      //if (Cache::Update(route, venue) == elle::StatusError)
      //escape("unable to update the cache");

      //printf("[XXX] /Path::Resolve()\n");
      //std::cout << address << std::endl;

      leave();
    }

    ///
    /// this method takes a slice and tries to extract both the real
    /// slice and the version number.
    ///
    /// for instance the slice 'teton.txt~42#'---assuming the regexp '~[0-9]+#'
    /// is used for version numbers---would be split into 'teton.txt' and
    /// the version number 42.
    ///
    elle::Status	Path::Parse(const Slab&			slab,
				    Slice&			slice,
				    nucleus::Version&		version)
    {
      enter();

      // XXX
      slice = slab;

      // XXX to remove the warning.
      version = nucleus::Version::Last;

      leave();
    }

  }
}
