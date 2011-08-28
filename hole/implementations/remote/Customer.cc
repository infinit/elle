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
// updated       julien quintard   [sun aug 28 21:40:32 2011]
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
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Customer::Customer():
	state(Customer::StateUnknown),
	gate(NULL)
      {
      }

      ///
      /// XXX
      ///
      Customer::~Customer()
      {
	// disconnect and delete the gate.
	if (this->gate != NULL)
	  delete this->gate;
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

	enter();

	// register the client.
	this->gate = gate;

	// register the error callback.
	if (this->gate->Monitor(monitor) == elle::StatusError)
	  escape("unable to monitor the connection");

	// XXX set up a timer after which the customer is discarded.

	leave();
      }

//
// ---------- callbacks -------------------------------------------------------
//

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

	leave();
      }

    }
  }
}
