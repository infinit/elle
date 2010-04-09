//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Network.cc
//
// created       julien quintard   [wed feb  3 16:49:46 2010]
// updated       julien quintard   [fri apr  9 01:15:53 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Network.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this container holds the list of registered callbacks.
    ///
    Network::Container		Network::Callbacks;

    ///
    /// this variable control the access to the network.
    ///
    Accord			Network::Control;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the network components.
    ///
    Status		Network::Initialize()
    {
      enter();

      // initialize the lane.
      if (Lane::Initialize() == StatusError)
	escape("unable to initialize the lane");

      // XXX bridge

      // initialize the session.
      if (Session::Initialize() == StatusError)
	escape("unable to initialize the session");

      leave();
    }

    ///
    /// this method cleans the network components.
    ///
    Status		Network::Clean()
    {
      enter();

      // clean the session.
      if (Session::Clean() == StatusError)
	escape("unable to clean the session");

      // clean the lane
      if (Lane::Clean() == StatusError)
	escape("unable to clean the lane");

      // XXX bridge

      /// XXX \todo clean the callbacks and receivers.

      leave();
    }

    ///
    /// this method takes a newly received packet and dispatch it.
    ///
    /// note that the input variables are not tracked for automatic
    /// deletion because the caller should already been tracking them.
    ///
    Status		Network::Dispatch(Parcel*		p)
    {
      Network::Scoutor		scoutor;
      Parcel*			parcel;

      enter(instance(parcel));

      // retrieve the argument and takes over the tracking.
      parcel = p;

      printf("[XXX] Network::Dispatch(tag[%u] event[%qu])\n",
	     parcel->header->tag, parcel->header->event.identifier);

      //
      // first, try to  wake up a waiting slot.
      //
      {
	// try to wake up a slot.
	if (Fiber::Awaken(parcel->header->event, parcel) == StatusTrue)
	  {
	    // since the awakening has been successful, stop tracking parcel.
	    waive(parcel);

	    leave();
	  }
      }

      //
      // if no slot is waiting for this event, dispatch it right away.
      //
      {
	// lock in reading.
	Network::Control.Lock(ModeRead);
	{
	  // retrieve the callback associated to the header's tag.
	  if ((scoutor = Network::Callbacks.find(parcel->header->tag)) ==
	      Network::Callbacks.end())
	    {
	      Network::Control.Unlock();

	      // unable to locate the callback, just ignore the message.
	      delete parcel;

	      // stop tracking the parcel since it has just been deleted.
	      waive(parcel);

	      leave();
	    }

	  // note that, at this point, the lock is release though the
	  // scoutor is still going to be used.
	  //
	  // this is necessary since the callbacks triggered may need to
	  // access the network and, for instance, register new callbacks
	  // etc.
	  //
	  // this should not be a problem since callbacks are, theoretically,
	  // never unregistered.
	}
	Network::Control.Unlock();

	// assign the new session.
	if (Session::Assign(parcel->session) == StatusError)
	  escape("unable to assign the session");

	// trigger the callback.
	if (scoutor->second->Call(*parcel->data) == StatusError)
	  escape("an error occured while processing the event");

	// clear the session.
	if (Session::Clear() == StatusError)
	  escape("unable to clear the session");

	// delete the parcel.
	delete parcel;

	// stop tracking the parcel since it has just been deleted.
	waive(parcel);
      }

      leave();
    }

    ///
    /// this method dumps the table of callbacks.
    ///
    Status		Network::Show(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      Network::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Network]" << std::endl;

      // lock in reading.
      Network::Control.Lock(ModeRead);
      {
	// dump the callbacks table.
	for (scoutor = Network::Callbacks.begin();
	     scoutor != Network::Callbacks.end();
	     scoutor++)
	  {
	    // dump the tag.
	    std::cout << alignment << Dumpable::Shift << "[Tag] "
		      << std::dec << scoutor->first << std::endl;

	    // dump the functionoid.
	    if (scoutor->second->Dump(margin + 2) == StatusError)
	      {
		Network::Control.Unlock();
		escape("unable to dump the functionoid");
	      }
	  }
      }
      Network::Control.Unlock();

      leave();
    }

  }
}
