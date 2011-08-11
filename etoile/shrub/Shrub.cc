//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/shrub/Shrub.cc
//
// created       julien quintard   [sat aug  6 17:48:20 2011]
// updated       julien quintard   [thu aug 11 12:44:58 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/shrub/Shrub.hh>

#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>
#include <Infinit.hh>

namespace etoile
{
  namespace shrub
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this container represents the root of the hierarchical data container.
    ///
    Riffle*			Shrub::Riffles = NULL;

    ///
    /// this queue maintains the freshness timestamps related to the riffles.
    ///
    /// whenever the capacity is reached, this data structure is used
    /// to quickly locate the least-recently-used riffles.
    ///
    Queue			Shrub::Timestamps;

    ///
    /// this timer triggers the sweeper on a regular basis.
    ///
    elle::Timer			Shrub::Timer;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the shrub.
    ///
    elle::Status	Shrub::Initialize()
    {
      elle::Callback< elle::Status,
		      elle::Parameters<> >	callback(&Shrub::Sweeper);

      enter();

      // create the sweeper timer.
      if (Shrub::Timer.Create(elle::Timer::ModeRepetition,
			      callback) == elle::StatusError)
	escape("unable to create the timer");

      // start the timer.
      if (Shrub::Timer.Start(Infinit::Configuration.shrub.frequence) ==
	  elle::StatusError)
	escape("unable to start the timer");

      leave();
    }

    ///
    /// this method cleans the shrub.
    ///
    elle::Status	Shrub::Clean()
    {
      enter();

      // delete the shrub content, if present.
      if (Shrub::Riffles != NULL)
	{
	  // clear the riffle.
	  if (Shrub::Riffles->Clear() == elle::StatusError)
	    escape("unable to clear the riffles");

	  // release the shrub slot.
	  if (Shrub::Timestamps.Remove(Shrub::Riffles) == elle::StatusError)
	    escape("unable to remove the riffle");

	  // delete the root riffle.
	  delete Shrub::Riffles;
	}

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Shrub::Allocate(const elle::Natural32	size)
    {
      elle::Natural32	i;

      enter();

      // release as many riffle as requested and possible.
      for (i = 0;
	   (i < size) && (Shrub::Timestamps.container.size() != 0);
	   i++)
	{
	  Riffle*		riffle;

	  // retrieve the least-recently-used riffle.
	  riffle = Shrub::Timestamps.container.begin()->second;

	  // depending on the riffle's parent.
	  if (riffle->parent != NULL)
	    {
	      // destroy the entry in the parent riffle.
	      if (riffle->parent->Destroy(riffle->slab) == elle::StatusError)
		escape("unable to destroy the riffle");
	    }
	  else
	    {
	      //
	      // otherwise, the root riffle needs to be released.
	      //

	      // clear the riffle.
	      if (Shrub::Riffles->Clear() == elle::StatusError)
		escape("unable to clear the riffles");

	      // release the shrub slot.
	      if (Shrub::Timestamps.Remove(Shrub::Riffles) ==
		  elle::StatusError)
		escape("unable to remove the riffle");

	      // delete the root riffle.
	      delete Shrub::Riffles;

	      // reset the pointer.
	      Shrub::Riffles = NULL;
	    }
	}

      leave();
    }

    ///
    /// this method takes a path in the form of a route and updates
    /// the venue as much as possible through the shrub.
    ///
    elle::Status	Shrub::Resolve(const path::Route&	route,
				       path::Venue&		venue)
    {
      Riffle*			riffle;
      path::Route::Scoutor	scoutor;

      enter();

      // make sure the shrub has been activated, return otherwise.
      if (Infinit::Configuration.shrub.status == false)
	leave();

      // make sure this root riffle is present.
      if (Shrub::Riffles == NULL)
	leave();

      // resolve the root directory by recording its location.
      if (venue.Record(Shrub::Riffles->location) == elle::StatusError)
	escape("unable to record the location");

      // for every element of the route.
      for (riffle = Shrub::Riffles,
	     scoutor = route.elements.begin() + 1;
	   scoutor != route.elements.end();
	   scoutor++)
	{
	  // try to resolve within this riffle.
	  if (riffle->Resolve(*scoutor, riffle) == elle::StatusError)
	    escape("unable to resolve the route");

	  // check the pointer.
	  if (riffle == NULL)
	    break;

	  // add the location to the venue.
	  if (venue.Record(riffle->location) == elle::StatusError)
	    escape("unable to record the location");
	}

      leave();
    }

    ///
    /// this method updates the shrub with a route/venue tuple.
    ///
    elle::Status	Shrub::Update(const path::Route&	route,
				      const path::Venue&	venue)
    {
      Riffle*			riffle;
      path::Route::Scoutor	r;
      path::Venue::Scoutor	v;

      enter();

      // make sure the shrub has been activated, return otherwise.
      if (Infinit::Configuration.shrub.status == false)
	leave();

      //
      // first, try to resolve the given route in order to know how
      // many slabs are unresolved and will thus be added to the shrub.
      //
      {
	path::Venue		_venue;

	// resolve the route.
	if (Shrub::Resolve(route, _venue) == elle::StatusError)
	  escape("unable to resolve the route");

	// if there is not enough slots to inject the unresolved
	// slabs...
	if (_venue.elements.size() >
	    (Infinit::Configuration.shrub.capacity -
	     Shrub::Timestamps.container.size()))
	  {
	    // allocate slots for those elements.
	    if (Shrub::Allocate(_venue.elements.size()) == elle::StatusError)
	      escape("unable to allocate the required slots");
	  }
      }

      // make sure the root riffle is present, if not create it.
      if (Shrub::Riffles == NULL)
	{
	  enter(instance(riffle));

	  // allocate a new root riffle.
	  riffle = new Riffle;

	  // create the riffle.
	  if (riffle->Create(route.elements[0],
			     venue.elements[0]) == elle::StatusError)
	    escape("unable to create the riffle");

	  // add the riffle to the queue.
	  if (Shrub::Timestamps.Add(riffle) == elle::StatusError)
	    escape("unable to add the riffle");

	  // set the root riffle.
	  Shrub::Riffles = riffle;

	  // waive.
	  waive(riffle);

	  // release.
	  release();
	}
      else
	{
	  // update the root riffle's location.
	  Shrub::Riffles->location = venue.elements[0];
	}

      // for every element of the route/venue.
      for (riffle = Shrub::Riffles,
	     r = route.elements.begin() + 1, v = venue.elements.begin() + 1;
	   (r != route.elements.end()) && (v != venue.elements.end());
	   r++, v++)
	{
	  // update the riffle with the new location.
	  if (riffle->Update(*r, *v) == elle::StatusError)
	    escape("unable to update the riffle");

	  // try to resolve within this riffle.
	  //
	  // note that the previous update may have led to no change
	  // so that resolving the slab fails.
	  if (riffle->Resolve(*r, riffle) == elle::StatusError)
	    escape("unable to resolve the route");

	  // check the pointer.
	  if (riffle == NULL)
	    break;
	}

      leave();
    }

    ///
    /// this method takes a route and removes it from the shrub.
    ///
    elle::Status	Shrub::Evict(const path::Route&		route)
    {
      Riffle*			riffle;
      path::Route::Scoutor	scoutor;

      enter();

      // make sure the shrub has been activated, return otherwise.
      if (Infinit::Configuration.shrub.status == false)
	leave();

      // make sure the root riffle is present.
      if (Shrub::Riffles == NULL)
	leave();

      // for every element of the route/venue.
      for (riffle = Shrub::Riffles,
	     scoutor = route.elements.begin() + 1;
	   scoutor < route.elements.end();
	   scoutor++)
	{
	  // try to resolve within this riffle.
	  //
	  // if this process fails, it would mean that the given route
	  // is not held in the shrub.
	  if (riffle->Resolve(*scoutor, riffle) == elle::StatusError)
	    escape("unable to resolve the route");

	  // check the pointer.
	  if (riffle == NULL)
	    leave();
	}

      //
      // at this point, _riffle_ points to the riffle associated with the
      // given route.
      //

      // destroy the riffle by removing it from its parent. should this parent
      // not exist---i.e for the root riffle---reset the root pointer.
      if (riffle->parent != NULL)
	{
	  // destroy the entry in the parent riffle.
	  if (riffle->parent->Destroy(riffle->slab) == elle::StatusError)
	    escape("unable to destroy the riffle's entry");
	}
      else
	{
	  //
	  // otherwise, the route references the root riffle.
	  //

	  // clear the riffle.
	  if (Shrub::Riffles->Clear() == elle::StatusError)
	    escape("unable to clear the riffles");

	  // release the shrub slot.
	  if (Shrub::Timestamps.Remove(Shrub::Riffles) == elle::StatusError)
	    escape("unable to remove the riffle");

	  // delete the root riffle.
	  delete Shrub::Riffles;

	  // reset the pointer.
	  Shrub::Riffles = NULL;
	}

      leave();
    }

    ///
    /// this method dumps the whole shrub.
    ///
    elle::Status	Shrub::Show(const elle::Natural32	margin)
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Shrub]" << std::endl;

      // make sure this root riffle is present.
      if (Shrub::Riffles != NULL)
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Riffles]" << std::endl;

	  // just initiate a recursive dump, starting with the root riffle.
	  if (Shrub::Riffles->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the shrub's riffles");
	}

      // dump the queue.
      if (Shrub::Timestamps.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the queue");

      leave();
    }

//
// ---------- static callbacks ------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status	Shrub::Sweeper()
    {
      elle::Duration	lifespan(elle::Duration::UnitSeconds,
				 Infinit::Configuration.shrub.lifespan);
      elle::Time	current;
      elle::Time	threshold;

      enter();

      // retrieve the current timestamp.
      if (current.Current() == elle::StatusError)
	escape("unable to retrieve the current time");

      // substract the lifespan to the current time rather than adding it
      // to the timestamp of every riffle.
      threshold = current - lifespan;

      // go through the queue as long as the riffles have expired i.e
      // their update timestamp has reached the threshold.
      while (Shrub::Timestamps.container.empty() == false)
	{
	  Riffle*		riffle;

	  // retrieve the least-recently-used riffle.
	  riffle = Shrub::Timestamps.container.begin()->second;

	  // if the riffle has not expire, exit the loop since all the
	  // following riffles are fresher.
	  if (riffle->timestamp > threshold)
	    break;

	  // depending on the riffle's parent.
	  if (riffle->parent != NULL)
	    {
	      // destroy this riffle.
	      if (riffle->parent->Destroy(riffle->slab) == elle::StatusError)
	        escape("unable to destroy the riffle");
	    }
	  else
	    {
	      //
	      // otherwise, the root riffle needs to be released.
	      //

	      // clear the riffle.
	      if (Shrub::Riffles->Clear() == elle::StatusError)
		escape("unable to clear the riffles");

	      // release the shrub slot.
	      if (Shrub::Timestamps.Remove(Shrub::Riffles) ==
		  elle::StatusError)
		escape("unable to remove the riffle");

	      // delete the root riffle.
	      delete Shrub::Riffles;

	      // reset the pointer.
	      Shrub::Riffles = NULL;
	    }
	}

      leave();
    }

  }
}
