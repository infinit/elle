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
// updated       julien quintard   [wed jul 13 19:56:23 2011]
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
    Registrar<Tag>			Network::Bureau;

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

      // recycle the bureau.
      if (Network::Bureau.Recycle< Registrar<Tag> >() == StatusError)
	escape("unable to recycle the bureau");

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
      Parcel*			parcel;

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
	// assign the new session.
	if (Session::Assign(parcel->session) == StatusError)
	  escape("unable to assign the session");

	// dispatch the call.
	if (Network::Bureau.Dispatch(parcel->header->tag,
				     *parcel->data) == StatusError)
	  {
	    // clear the session.
	    if (Session::Clear() == StatusError)
	      escape("unable to clear the session");

	    escape("unable to dispatch the event through the registrar");
	  }

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
    /// this method dumps the bureau.
    ///
    Status		Network::Show(const Natural32		margin)
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Network]" << std::endl;

      // dump the registrar.
      if (Network::Bureau.Dump(margin + 2) == StatusError)
	escape("unable to dump the registrar");

      leave();
    }

  }
}
