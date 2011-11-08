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

#include <hole/implementations/slug/Host.hh>
#include <hole/implementations/slug/Slug.hh>
#include <hole/implementations/slug/Manifest.hh>
#include <hole/Hole.hh>

#include <Infinit.hh>

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
      /// XXX 2 sec
      ///
      const elle::Natural32		Host::Timeout = 1000;

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Host::Host():
	state(StateUnknown),
	gate(NULL),
	timer(NULL)
      {
      }

      ///
      /// XXX
      ///
      Host::~Host()
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
      elle::Status	Host::Create(const elle::Locus&		locus)
      {
	enter();

	// set the locus.
	this->locus = locus;

	// allocate a gate.
	this->gate = new elle::Gate;

	// create the gate.
	if (this->gate->Create() == elle::StatusError)
	  escape("unable to create the gate");

	// allocate the timer.
	this->timer = new elle::Timer;

	// create the timer.
	if (this->timer->Create(elle::Timer::ModeSingle) == elle::StatusError)
	  escape("unable to create the timer");

	// subscribe to the timer's signal.
	if (this->timer->signal.timeout.Subscribe(
	      elle::Callback<>::Infer(&Host::Abort,
				      this)) == elle::StatusError)
	  escape("unable to subscribe to the signal");

	// start the timer.
	if (this->timer->Start(Host::Timeout) == elle::StatusError)
	  escape("unable to start the timer");

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Host::Create(elle::Gate*		gate)
      {
	enter();

	// set the gate.
	this->gate = gate;

	// set the state.
	this->state = Host::StateConnected;

	// subscribe to the signal.
        if (this->gate->signal.disconnected.Subscribe(
              elle::Callback<>::Infer(&Host::Disconnected,
                                      this)) == elle::StatusError)
          escape("unable to subscribe the signal");

	// subscribe to the signal.
        if (this->gate->signal.error.Subscribe(
              elle::Callback<>::Infer(&Host::Error,
                                      this)) == elle::StatusError)
          escape("unable to subscribe the signal");

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Host::Connect()
      {
	enter();

	// subscribe to the signal.
        if (this->gate->signal.connected.Subscribe(
              elle::Callback<>::Infer(&Host::Connected,
                                      this)) == elle::StatusError)
          escape("unable to subscribe the signal");

	// subscribe to the signal.
        if (this->gate->signal.disconnected.Subscribe(
              elle::Callback<>::Infer(&Host::Disconnected,
                                      this)) == elle::StatusError)
          escape("unable to subscribe the signal");

	// subscribe to the signal.
        if (this->gate->signal.error.Subscribe(
              elle::Callback<>::Infer(&Host::Error,
                                      this)) == elle::StatusError)
          escape("unable to subscribe the signal");

	// connect the gate.
	if (this->gate->Connect(this->locus) == elle::StatusError)
	  escape("unable to connect to the peer");

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Host::Disconnect()
      {
	enter();

	// disconnect the gate.
	if (this->gate->Disconnect() == elle::StatusError)
	  escape("unable to disconnect the socket");

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Host::Authenticated()
      {
	enter();

	// delete the timer.
	delete this->timer;

	// reset the pointer.
	this->timer = NULL;

	// set the state.
	this->state = Host::StateAuthenticated;

	leave();
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status	Host::Abort()
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Host::Abort()\n");

	// delete the timer.
	bury(this->timer);

	// reset the pointer.
	this->timer = NULL;

	// if the host has not been authenticated...
	if (this->state != Host::StateAuthenticated)
	  {
	    // emit the signal.
	    if (this->signal.dead.Emit(this) == elle::StatusError)
	      escape("unable to emit the signal");

	    // set the state.
	    this->state = Host::StateDead;
	  }

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Host::Connected()
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Host::Connected()\n");

	// set the state.
	this->state = Host::StateConnected;

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Host::Disconnected()
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Host::Disconnected()\n");

	// emit the signal.
	if (this->signal.dead.Emit(this) == elle::StatusError)
	  escape("unable to emit the signal");

	// set the state.
	this->state = Host::StateDead;

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Host::Error(const elle::String&)
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Host::Error()\n");

	//
	// nothing to do.
	//

	leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this function dumps a host object.
      ///
      elle::Status	Host::Dump(elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	// display the name.
	std::cout << alignment << "[Host] "
		  << std::hex << this << std::endl;

	// display the state.
	std::cout << alignment << elle::Dumpable::Shift
		  << "[State] " << std::dec << this->state << std::endl;

	// dump the locus.
	if (this->locus.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the locus");

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

	// dump the timer, if present.
	if (this->timer != NULL)
	  {
	    if (this->timer->Dump(margin + 2) == elle::StatusError)
	      escape("unable to dump the timer");
	  }
	else
	  {
	    std::cout << alignment << elle::Dumpable::Shift
		      << "[Timer] " << elle::none << std::endl;
	  }

	leave();
      }

    }
  }
}
