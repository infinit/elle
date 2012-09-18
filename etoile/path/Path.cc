#include <etoile/path/Path.hh>
#include <etoile/path/Venue.hh>
#include <etoile/path/Route.hh>
#include <etoile/path/Chemin.hh>
#include <etoile/gear/Identifier.hh>
#include <etoile/wall/Directory.hh>
#include <etoile/depot/Depot.hh>
#include <etoile/shrub/Shrub.hh>

#include <nucleus/proton/Revision.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/neutron/Entry.hh>

#include <hole/Hole.hh>

#include <Infinit.hh>

#include <elle/idiom/Close.hh>
# include <boost/lexical_cast.hpp>
#include <elle/idiom/Open.hh>

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
    elle::Status        Path::Initialize()
    {
      // initialize the route.
      if (Route::Initialize() == elle::Status::Error)
        escape("unable to initialize the route");

      return elle::Status::Ok;
    }

    ///
    /// this method cleans the path system.
    ///
    elle::Status        Path::Clean()
    {
      // clean the route.
      if (Route::Clean() == elle::Status::Error)
        escape("unable to clean the route");

      return elle::Status::Ok;
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
    elle::Status        Path::Resolve(const Route&              route,
                                      Venue&                    venue)
    {
      nucleus::proton::Address address;
      nucleus::proton::Revision revision;
      Route::Scoutor    scoutor;

      // first ask the shrub i.e path cache to resolve as much as it can.
      if (shrub::Shrub::Resolve(route, venue) == elle::Status::Error)
        escape("unable to resolve part of the route through the shrub");

      route.Dump();
      venue.Dump();
      assert(venue.elements.size() <= route.elements.size());

      // if complete, return the address i.e without updating the cache.
      if (route.elements.size() == venue.elements.size())
        return elle::Status::Ok;

      // if the cache did not resolve anything.
      if (venue == Venue::Null)
        {
          Slice         slice;

          // retrieve the root directory's address.
          if (depot::Depot::Origin(address) == elle::Status::Error)
            escape("unable to retrieve the address of the root directory");

          // parse the very first slab i.e the root slab in order
          // to extract the revision number. note that the root slab is
          // always empty.
          if (Path::Parse(route.elements[0],
                          slice, revision) == elle::Status::Error)
            escape("unable to extract the revision number from the root slab");

          // check that the slice is empty, as it should for the root
          // directory.
          if (slice.empty() == false)
            escape("the root slice should always be empty");

          // record the root directory in the venue.
          if (venue.Record(address, revision) == elle::Status::Error)
            escape("unable to record the root directory in the venue");
        }

      // set the address/revision with the address of the last resolved element.
      address = venue.elements[venue.elements.size() - 1].address;
      revision = venue.elements[venue.elements.size() - 1].revision;

      // otherwise, resolve manually by retrieving the directory object.
      for (scoutor = route.elements.begin() + venue.elements.size();
           scoutor != route.elements.end();
           scoutor++)
        {
          Chemin                chemin;
          Slice                 slice;
          gear::Identifier      identifier;
          nucleus::neutron::Entry const* entry;

          // extract the slice/revision from the current slab.
          if (Path::Parse(*scoutor,
                          slice,
                          revision) == elle::Status::Error)
            escape("unable to extract the slice/revision from the "
                   "current slab");

          // check that the slice is not empty.
          if (slice.empty() == true)
            escape("the slice should never be empty");

          // create the chemin.
          if (chemin.Create(route, venue,
                            venue.elements.size()) == elle::Status::Error)
            escape("unable to create the chemin");

          // load the directory.
          if (wall::Directory::Load(chemin, identifier) == elle::Status::Error)
            escape("unable to load the directory");

          // lookup the slice.
          if (wall::Directory::Lookup(identifier,
                                      slice,
                                      entry) == elle::Status::Error)
            {
              // discard the directory.
              if (wall::Directory::Discard(identifier) == elle::Status::Error)
                escape("unable to discard the directory");

              escape("unable to lookup the slice");
            }

          // set the address; the revision is already set i.e it has
          // been extracted from the slab.
          if (entry != nullptr)
            address = entry->address;

          // discard the directory.
          if (wall::Directory::Discard(identifier) == elle::Status::Error)
            escape("unable to discard the directory");

          // if there is no such entry, abort.
          //
          // note that the pointer is used to know whether or not the
          // lookup has succeded. however, the entry's content cannot be
          // accessed as it has potentially been released with the context
          // through the call to Discard().
          if (entry == nullptr)
            escape("unable to locate the directory entry '%s'",
                   slice.c_str());

          // first, record the address/revision in the venue.
          if (venue.Record(address, revision) == elle::Status::Error)
            escape("unable to record the venue address");
        }

      // update the shrub with the resolved path.
      if (shrub::Shrub::Update(route, venue) == elle::Status::Error)
        escape("unable to update the shrub");

      return elle::Status::Ok;
    }

    ///
    /// this method takes a slice and tries to extract both the real
    /// slice and the revision number.
    ///
    /// for instance the slice 'teton.txt%42'---assuming the regexp '%[0-9]+'
    /// is used for revision numbers---would be split into 'teton.txt' and
    /// the revision number 42.
    ///
    elle::Status        Path::Parse(const Slab&                 slab,
                                    Slice&                      slice,
                                    nucleus::proton::Revision& revision)
    {
      Length                    length;
      size_t                    start;
      nucleus::proton::Revision::Type n;

      // if the history mechanism is not supported by the network or
      // has not been activated through the user's configuration, return.
      if ((hole::Hole::Descriptor.meta().history() == false) ||
          (Infinit::Configuration.etoile.history.status == false))
        {
          // set the slice as being the entire slab.
          slice = slab;

          // and set the revision as being the latest possible.
          revision = nucleus::proton::Revision::Last;
        }
      else
        {
          //
          // otherwise, try to handle the history parsing.
          //

          // compute the start index, should the in-path revisioning be
          // activated.
          if (Infinit::Configuration.etoile.history.status == true)
            {
              // try to locate the start index for the revision number.
              start = slab.find_last_of(
                        Infinit::Configuration.etoile.history.indicator.slab);
            }
          else
            {
              // set the start index as invalid since the user's
              // configuration has not activated this functionality.
              start = elle::String::npos;
            }

          // if a revision seems to have been found.
          if (start != elle::String::npos)
            {
              // retrieve the slab's length.
              length = slab.length();

              // retrieve the slice.
              slice = elle::String(slab, 0, start);

              try
                {
                  n = boost::lexical_cast<nucleus::proton::Revision::Type>(
                      elle::String(slab, start + 1, length - (start + 1))
                  );
                }
              catch (std::exception const& err)
                {
                  escape("unable to retreive the revision number: %s", err.what());
                }

              if (revision.Create(n) == elle::Status::Error)
                escape("unable to create the revision");
            }
          else
            {
              //
              // otherwise, act as if history was not supported by the
              // network.
              //

              // set the slice as being the entire slab.
              slice = slab;

              // and set the revision as being the latest possible.
              revision = nucleus::proton::Revision::Last;
            }
        }

      return elle::Status::Ok;
    }

  }
}
