//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed aug 24 13:12:46 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/slug/Neighbour.hh>
#include <hole/implementations/slug/Slug.hh>
#include <hole/implementations/slug/Manifest.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

      //
      // ---------- definitions -----------------------------------------------------
      //

      ///
      /// XXX 30 sec
      ///
      const elle::Natural32		Neighbour::Timeout = 30000;

      //
      // ---------- constructors & destructors --------------------------------------
      //

      ///
      /// XXX
      ///
      Neighbour::Neighbour():
	state(StateUnauthenticated),
	port(0),
	gate(NULL),
	timer(NULL)
      {
      }

      ///
      /// XXX
      ///
      Neighbour::~Neighbour()
      {
	// delete the gate, if present.
	if (this->gate != NULL)
	  delete this->gate;

	// delete the timer, if present.
	if (this->timer != NULL)
	  delete this->timer;
      }

      //
      // ---------- methods ---------------------------------------------------------
      //

      ///
      /// XXX
      ///
      elle::Status	Neighbour::Create(const elle::Locus&	locus)
      {
	enter();

	// set the locus.
	this->locus = locus;

	// allocate the timer.
	this->timer = new elle::Timer;

	// create the timer.
	if (this->timer->Create(elle::Timer::ModeSingle) == elle::StatusError)
	  escape("unable to create the timer");

	// XXX -discard +abort

	// subscribe to the timer's signal.
	if (this->timer->signal.timeout.Subscribe(
	      elle::Callback<>::Infer(&Neighbour::Discard,
				      this)) == elle::StatusError)
	  escape("unable to subscribe to the signal");

	// start the timer.
	if (this->timer->Start(Neighbour::Timeout) == elle::StatusError)
	  escape("unable to start the timer");

	leave();
      }

      elle::Status      Neighbour::RegisterCallbacks()
      {
        enter();

        if (this->gate->signal.disconnected.Subscribe(
              elle::Callback<>::Infer(&Neighbour::Disconnected,
                                      this)) != elle::StatusError)
          escape("unable to subscribe the disconnected signal");

        if (this->gate->signal.connected.Subscribe(
              elle::Callback<>::Infer(&Neighbour::Connected,
                                      this)) != elle::StatusError)
          escape("unable to subscribe the disconnected signal");

        if (this->gate->signal.error.Subscribe(
              elle::Callback<>::Infer(&Neighbour::Error,
                                      this)) != elle::StatusError)
          escape("unable to subscribe the disconnected signal");

        leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Neighbour::Create(elle::Gate*		gate)
      {
	enter();

	// set the gate.
	this->gate = gate;

        this->RegisterCallbacks();

	// XXX add timer for timeout

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Neighbour::Connect()
      {
	enter();

	// allocate a gate.
	this->gate = new elle::Gate;

        this->RegisterCallbacks();

	// create the gate.
	if (this->gate->Create() == elle::StatusError)
	  escape("unable to create the gate");

	// connect the gate.
	if (this->gate->Connect(this->locus) == elle::StatusError)
	  escape("unable to connect to the peer");

	leave();
      }

      //
      // ---------- callbacks -------------------------------------------------------
      //

      ///
      /// XXX
      ///
      elle::Status	Neighbour::Discard()
      {
	enter();

	// if the neighbour has been authenticated...
	if (this->state == Neighbour::StateAuthenticated)
	  {
	    // delete the timer.
	    delete this->timer;

	    // reset the locus.
	    this->timer = NULL;
	  }
	else
	  {
	    // remove the neighbour from the neighbourhood.
	    if (Slug::Computer->neighbourhood.Remove(
		  this->locus) == elle::StatusError)
	      escape("unable to remove the neighbour from the "
		     "neighbourhood");

	    // delete the neighbour.
	    delete this; // XXX rely on signals instead
	  }

	leave();
      }

      elle::Status	Neighbour::Connected()
      {
	enter();

	// challenge the peer.
	if (this->gate->Send(
	      elle::Inputs<TagChallenge>()) == elle::StatusError)
	  escape("unable to send the challenge");

	leave();
      }

      elle::Status	Neighbour::Disconnected()
      {
	enter();

	// if the neighbour has been registered in the routing table...
	if (Slug::Computer->routingtable.Exist(
	      this->label) == elle::StatusTrue)
	  {
	    // remove it.
	    if (Slug::Computer->routingtable.Remove(
		  this->label) == elle::StatusError)
	      escape("unable to remove the neighbour from the "
		     "routing table");
	  }

	// remove the neighbour from the neighbourhood.
	if (Slug::Computer->neighbourhood.Remove(
	      this->locus) == elle::StatusError)
	  escape("unable to remove the neighbour from the "
		 "neighbourhood");

	// delete the neighbour.
	delete this; // XXX rely on signals instead

	leave();
      }

      elle::Status	Neighbour::Error(const elle::String & error)
      {
	enter();

	this->Disconnected();

	leave();
      }

      //
      // ---------- dumpable --------------------------------------------------------
      //

      ///
      /// this function dumps a neighbour object.
      ///
      elle::Status	Neighbour::Dump(elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	// display the name.
	std::cout << alignment << "[Neighbour] "
		  << std::hex << this << std::endl;

	// display the state.
	std::cout << alignment << elle::Dumpable::Shift << "[State] "
		  << this->state << std::endl;

	// dump the locus.
	if (this->locus.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the locus");

	// dump the label.
	if (this->label.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the label");

	// dump the port.
	std::cout << alignment << elle::Dumpable::Shift
		  << "[Port] " << std::dec << this->port << std::endl;

	// dump the gate, if present.
	if (this->gate != NULL)
	  {
	    if (this->gate->Dump(margin + 2) == elle::StatusError)
	      escape("unable to dump the gate");
	  }
	else
	  {
	    std::cout << alignment << elle::Dumpable::Shift
		      << "[Gate] " << elle::none << std::endl;
	  }

	leave();
      }

    }
  }
}
