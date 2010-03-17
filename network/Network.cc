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
// updated       julien quintard   [tue mar 16 22:45:33 2010]
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
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the network components.
    ///
    Status		Network::Initialize()
    {
      enter();

      // initialize the bridge.
      if (Bridge::Initialize() == StatusError)
	escape("unable to initialize the bridge");

      leave();
    }

    ///
    /// this method cleans the network components.
    ///
    Status		Network::Clean()
    {
      enter();

      // clean the bridge.
      if (Bridge::Clean() == StatusError)
	escape("unable to clean the bridge");

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

      //
      // first, try to  wake up a waiting slot.
      //
      {
	// try to wake up a slot.
	if (Application::Awake(parcel->header->identifier,
			       parcel) == StatusTrue)
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
	      escape("unable to locate the callback");
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

	// assign the new context.
	if (Context::Assign(parcel->context) == StatusError)
	  escape("unable to assign the context");

	// trigger the callback.
	if (scoutor->second->Call(*parcel->data) == StatusError)
	  escape("unable to dispatch the event");

	// delete the parcel.
	delete parcel;

	// stop tracking the parcel since it has just been deleted.
	waive(parcel);
      }

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the table of callbacks.
    ///
    Status		Network::Dump(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');
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
	    std::cout << alignment << shift << "[Tag] "
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
