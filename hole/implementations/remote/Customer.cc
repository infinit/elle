//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [sun aug 28 17:53:05 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Customer.hh>
#include <hole/implementations/remote/Remote.hh>

#include <Infinit.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

//
// ---------- definitions -----------------------------------------------------
//

      ///
      /// this constant defines the time after which a customer should
      /// have authenticated.
      ///
      const elle::Natural32		Customer::Timeout = 180000;

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// default constructor.
      ///
      Customer::Customer():
	state(Customer::StateUnknown),
	gate(NULL),
	timer(NULL)
      {
      }

      ///
      /// destructor.
      ///
      Customer::~Customer()
      {
	// delete the gate.
	if (this->gate != NULL)
	  delete this->gate;

	// delete the timer.
	if (this->timer != NULL)
	  delete this->timer;
      }

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// this method creates a customer from the given socket.
      ///
      elle::Status	Customer::Create(elle::Gate*		gate)
      {
	enter();

	// register the client.
	this->gate = gate;

	// subscribe to the signal.
	if (this->gate->signal.disconnected.Subscribe(
	      elle::Callback<>::Infer(&Customer::Disconnected,
				      this)) == elle::StatusError)
	  escape("unable to subscribe to the signal");

	// subscribe to the signal.
	if (this->gate->signal.error.Subscribe(
	      elle::Callback<>::Infer(&Customer::Error,
				      this)) == elle::StatusError)
	  escape("unable to subscribe to the signal");

	// allocate a new timer.
	this->timer = new elle::Timer;

	// create the timer.
	if (this->timer->Create(elle::Timer::ModeSingle) == elle::StatusError)
	  escape("unable to create the timer");

	// subscribe to the timer's signal.
	if (this->timer->signal.timeout.Subscribe(
	      elle::Callback<>::Infer(&Customer::Abort,
				      this)) == elle::StatusError)
	  escape("unable to subscribe to the signal");

	// start the timer.
	if (this->timer->Start(Customer::Timeout) == elle::StatusError)
	  escape("unable to start the timer");

	leave();
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// this callback is triggered whenever the customer is considered
      /// disconnected.
      ///
      elle::Status	Customer::Disconnected()
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::remote::Customer::Disconnected()");

	// set the customer's state as dead.
	this->state = Customer::StateDead;

	// emit the signal.
	if (this->signal.dead.Emit(this) == elle::StatusError)
	  escape("unable to emit the signal");

	leave();
      }

      ///
      /// this callback is triggered whenever an error occurs on the
      /// customer's socket.
      ///
      elle::Status	Customer::Error(const elle::String&	error)
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::remote::Customer::Error()");

	// log the error.
	log(error.c_str());

	// disconnect the gate, though that may be unecessary.
	this->gate->Disconnect();

	leave();
      }

      ///
      /// this callback is triggered once the authentication timer times out.
      ///
      /// if the customer has not been authenticated, it is destroyed.
      ///
      elle::Status	Customer::Abort()
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::remote::Customer::Abort()");

	// delete the timer.
	delete this->timer;

	// reset the pointer.
	this->timer = NULL;

	// check if the customer has been authenticated.
	if (this->state != Customer::StateAuthenticated)
	  {
	    // emit the signal.
	    if (this->signal.dead.Emit(this) == elle::StatusError)
	      escape("unable to emit the signal");
	  }

	leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the customer.
      ///
      elle::Status	Customer::Dump(const elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Customer]" << std::endl;

	// dump the state.
	std::cout << alignment << elle::Dumpable::Shift
		  << "[State] " << this->state << std::endl;

	// dump the gate.
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

	// dump the timer.
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
