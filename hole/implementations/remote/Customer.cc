//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Customer.cc
//
// created       julien quintard   [sun aug 28 17:53:05 2011]
// updated       julien quintard   [mon aug 29 09:48:11 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Customer.hh>
#include <hole/implementations/remote/Remote.hh>

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
      /// XXX
      ///
      const elle::Natural32		Customer::Timeout = 180000;

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Customer::Customer():
	state(Customer::StateUnknown),
	gate(NULL),
	timer(NULL)
      {
      }

      ///
      /// XXX
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
      /// XXX
      ///
      elle::Status	Customer::Create(elle::Gate*		gate)
      {
	elle::Callback<
	  elle::Status,
	  elle::Parameters<>
	>				monitor(&Customer::Monitor, this);
	elle::Callback<
	  elle::Status,
	  elle::Parameters<>
	>				abort(&Customer::Abort, this);

	enter();

	// register the client.
	this->gate = gate;

	// register the error callback.
	if (this->gate->Monitor(monitor) == elle::StatusError)
	  escape("unable to monitor the connection");

	// allocate a new timer.
	this->timer = new elle::Timer;

	// create the timer.
	if (this->timer->Create(elle::Timer::ModeSingle,
				abort) == elle::StatusError)
	  escape("unable to create the timer");

	// start the timer.
	if (this->timer->Start(Customer::Timeout) == elle::StatusError)
	  escape("unable to start the timer");

	leave();
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status	Customer::Abort()
      {
	enter();

	// reset the state.
	this->state = Customer::StateUnknown;

	// remove the customer from the server.
	if (Remote::Machine->server->Remove(this->gate) == elle::StatusError)
	  escape("unable to remove the customer");

	// bury the customer.
	bury(this);

	leave();
      }

      ///
      /// this callback is triggered whenever something unexpected occurs
      /// on the connection.
      ///
      elle::Status	Customer::Monitor()
      {
	enter();

	// if the customer's connection has been shut down...
	if (this->gate->state == elle::Channel::StateDisconnected)
	  {
	    // reset the state.
	    this->state = Customer::StateUnknown;

	    // remove the customer from the server.
	    if (Remote::Machine->server->Remove(this->gate) ==
		elle::StatusError)
	      escape("unable to remove the customer");

	    // bury the customer.
	    bury(this);
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
