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
// updated       julien quintard   [sat sep 10 15:41:34 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Path.hh>

#include <etoile/gear/Directory.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Actor.hh>

#include <etoile/automaton/Directory.hh>

#include <etoile/depot/Depot.hh>

#include <etoile/shrub/Shrub.hh>

#include <agent/Agent.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the path system.
    ///
    elle::Status	Path::Initialize()
    {
      enter();

      // initialize the route.
      if (Route::Initialize() == elle::StatusError)
	escape("unable to initialize the route");

      leave();
    }

    ///
    /// this method cleans the path system.
    ///
    elle::Status	Path::Clean()
    {
      enter();

      // clean the route.
      if (Route::Clean() == elle::StatusError)
	escape("unable to clean the route");

      leave();
    }

    ///
    /// this method takes a route and returns the address of the
    /// referenced object.
    ///
    /// this method starts by resolving the route by looking up in the
    /// shrub. then, the resolving process retrieved the uncached directory
    /// objects and explore them.
    ///
    /// note that this method only processes absolute paths. paths being
    /// composed of links will fail to be resolved for instance.
    ///
    elle::Status	Path::Resolve(const Route&		route,
				      Venue&			venue)
    {
      nucleus::Address	address;
      nucleus::Version	version;
      Route::Scoutor	scoutor;

      enter();

      // first ask the shrub i.e path cache to resolve as much as it can.
      if (shrub::Shrub::Resolve(route, venue) == elle::StatusError)
	escape("unable to resolve part of the route through the shrub");

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

	  // check that the slice is empty, as it should for the root
	  // directory.
	  if (slice.empty() == false)
	    escape("the root slice should always be empty");

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

	  Chemin		chemin;
	  gear::Scope*		scope;
	  gear::Directory*	context;
	  gear::Actor*		actor;
	  Slice			slice;
	  nucleus::Entry*	entry;
	  nucleus::Location	location;

	  enter(instance(actor));

	  // extract the slice/version from the current slab.
	  if (Path::Parse(*scoutor,
			  slice,
			  version) == elle::StatusError)
	    escape("unable to extract the slice/version from the "
		   "current slab");

	  // check that the slice is not empty.
	  if (slice.empty() == true)
	    escape("the slice should never be empty");

	  // create the chemin.
	  if (chemin.Create(route, venue,
			    venue.elements.size()) == elle::StatusError)
	    escape("unable to create the chemin");

	  // acquire the scope.
	  if (gear::Scope::Acquire(chemin, scope) == elle::StatusError)
	    escape("unable to acquire the scope");

	  // retrieve the context.
	  if (scope->Use(context) == elle::StatusError)
	    escape("unable to retrieve the context");

	  // allocate an actor.
	  actor = new gear::Actor(scope);

	  // attach the actor to the scope.
	  if (actor->Attach() == elle::StatusError)
	    escape("unable to attach the actor to the scope");

	  // create the location.
	  if (location.Create(address, version) == elle::StatusError)
	    escape("unable to create the location");

	  // fetch the directory.
	  if (automaton::Directory::Load(*context,
					 location) == elle::StatusError)
	    escape("unable to fetch the directory object");

	  // look up for the name.
	  if (automaton::Directory::Lookup(*context,
					   slice,
					   entry) == elle::StatusError)
	    escape("unable to find one of the route's entries");

	  // set the address; the version is already set i.e it has
	  // been extracted from the slab.
	  if (entry != NULL)
	    address = entry->address;

	  // specify the closing operation performed on the scope.
	  if (actor->scope->Operate(gear::OperationDiscard) ==
	      elle::StatusError)
	    escape("unable to specify the operation being performed "
		   "on the scope");

	  // detach the actor.
	  if (actor->Detach() == elle::StatusError)
	    escape("unable to detach the actor from the scope");

	  // relinquish the scope.
	  if (gear::Scope::Relinquish(actor->scope) == elle::StatusError)
	    escape("unable to relinquish the scope");

	  // delete the actor.
	  delete actor;

	  // waive the actor.
	  waive(actor);

	  // if there is no such entry, abort.
	  //
	  // note that the pointer is used to know whether or not the
	  // lookup has succeded. however, the entry's content cannot be
	  // accessed as it has potentially been released with the context
	  // through the call to Relinquish().
	  if (entry == NULL)
	    escape("unable to locate the directory entry '%s'",
		   slice.c_str());

	  // first, record the address/version in the venue.
	  if (venue.Record(address, version) == elle::StatusError)
	    escape("unable to record the venue address");

	  // release the resources.
	  release();
	}

      // update the shrub with the resolved path.
      if (shrub::Shrub::Update(route, venue) == elle::StatusError)
	escape("unable to update the shrub");

      leave();
    }

    ///
    /// this method takes a slice and tries to extract both the real
    /// slice and the version number.
    ///
    /// for instance the slice 'teton.txt%42'---assuming the regexp '%[0-9]+'
    /// is used for version numbers---would be split into 'teton.txt' and
    /// the version number 42.
    ///
    elle::Status	Path::Parse(const Slab&			slab,
				    Slice&			slice,
				    nucleus::Version&		version)
    {
      Length			length;
      size_t			start;
      nucleus::Version::Type	n;

      enter();

      // compute the start index.
      start = slab.find_last_of(
	        Infinit::Configuration.history.indicator.slab);

      // if the in-path versioning has been activated and a version
      // seems to have been found.
      if ((Infinit::Configuration.history.status == true) &&
	  (start != elle::String::npos))
	{
	  // retrieve the slab's length.
	  length = slab.length();

	  // retrieve the slice.
	  slice = elle::String(slab, 0, start);

	  // transform the string into a number.
	  if (elle::Variable::Convert(elle::String(slab,
						   start + 1,
						   length - (start + 1)),
				      n) == elle::StatusError)
	    escape("unable to convert the string-based version number");

	  // create the version.
	  if (version.Create(n) == elle::StatusError)
	    escape("unable to create the version");
	}
      else
	{
	  // set the slice as being the entire slab.
	  slice = slab;

	  // and set the version as being the latest possible.
	  version = nucleus::Version::Last;
	}

      leave();
    }

  }
}
