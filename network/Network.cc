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
// updated       julien quintard   [sat sep  3 08:03:53 2011]
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
    /// this container holds the list of registered procedures.
    ///
    Network::Container			Network::Procedures;

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

      // initialize the bridge.
      if (Bridge::Initialize() == StatusError)
	escape("unable to initialize the bridge");

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

      // clean the bridge.
      if (Bridge::Clean() == StatusError)
	escape("unable to clean the bridge");

      // clean the lane
      if (Lane::Clean() == StatusError)
	escape("unable to clean the lane");

      //
      // clear the container.
      //
      {
	Network::Scoutor	scoutor;

	// go through the functionoids.
	for (scoutor = Network::Procedures.begin();
	     scoutor != Network::Procedures.end();
	     scoutor++)
	  {
	    // delete the functionoid.
	    delete scoutor->second;
	  }

	// clear the container.
	Network::Procedures.clear();
      }

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
      Parcel*		parcel;

      enter(instance(parcel));

      // retrieve the argument and takes over the tracking.
      parcel = p;

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
	Network::Scoutor	scoutor;

        // retrieve the procedure's functionoid associated to the header's tag.
	if ((scoutor = Network::Procedures.find(parcel->header->tag)) ==
	    Network::Procedures.end())
	  {
	    // test if the message received is an error, if so, log it.
	    if (parcel->header->tag == TagError)
	      {
		Report	report;

		// extract the error message.
		if (report.Extract(*parcel->data) == StatusError)
		  escape("unable to extract the error message");

		// report the remote error.
		transpose(report);

		// log the error.
		log("an error message has been received with no registered "
		    "procedure");
	      }

	    leave();
	  }

	// assign the new session.
	if (Session::Assign(parcel->session) == StatusError)
	  escape("unable to assign the session");

        // call the functionoid.
        if (scoutor->second->Call(*parcel->data) == StatusError)
	  escape("an error occured while processing the event");

	// clear the session.
	if (Session::Clear() == StatusError)
	  escape("unable to flush the session");

	// delete the parcel.
	delete parcel;

	// stop tracking the parcel since it has just been deleted.
	waive(parcel);
      }

      leave();
    }

    ///
    /// this method dumps the bureau.
    ///
    Status		Network::Show(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      Network::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Network]" << std::endl;

      // go through the functionoids.
      for (scoutor = Network::Procedures.begin();
	   scoutor != Network::Procedures.end();
	   scoutor++)
	{
	  // dump the functionoid.
	  if (scoutor->second->Dump(margin + 2) == StatusError)
	    escape("unable to dump the functionoid");
	}

      leave();
    }

  }
}
