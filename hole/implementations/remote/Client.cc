//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Client.cc
//
// created       julien quintard   [thu may 26 10:22:03 2011]
// updated       julien quintard   [wed aug 31 22:08:43 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Client.hh>
#include <hole/implementations/remote/Manifest.hh>

#include <hole/Hole.hh>

#include <Infinit.hh>

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
      /// default constructor.
      ///
      Client::Client(const elle::Point&				point):
	state(Client::StateUnknown),
	point(point),
	gate(NULL)
      {
      }

      ///
      /// the destructor.
      ///
      Client::~Client()
      {
	// delete the gate.
	if (this->gate != NULL)
	  delete this->gate;
      }

//
// ---------- peer ------------------------------------------------------------
//

      ///
      /// this method launches the client.
      ///
      elle::Status	Client::Launch()
      {
	enter();

	//
	// register the messages.
	//
	{
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<>
	    >				challenge(&Client::Challenge, this);
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<>
	    >				authenticated(&Client::Authenticated,
						      this);
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<
	      const elle::Report
	      >
	    >				error(&Client::Error, this);

	  // register the challenge message.
	  if (elle::Network::Register(
	        elle::Procedure<TagChallenge>(challenge)) == elle::StatusError)
	    escape("unable to register the callback");

	  // register the challenge message.
	  if (elle::Network::Register(
	        elle::Procedure<TagAuthenticated>(authenticated)) ==
	      elle::StatusError)
	    escape("unable to register the callback");

	  // register the challenge message.
	  if (elle::Network::Register(
	        elle::Procedure<TagError>(error)) == elle::StatusError)
	    escape("unable to register the callback");
	}

	//
	// connect to the server.
	//
	{
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<>
	    >				monitor(&Client::Monitor, this);

	  // allocate the gate.
	  this->gate = new elle::Gate;

	  // register the error callback.
	  if (this->gate->Monitor(monitor) == elle::StatusError)
	    escape("unable to monitor the connection");

	  // create the gate.
	  if (this->gate->Create() == elle::StatusError)
	    escape("unable to create the gate");

	  // connect the gate.
	  if (this->gate->Connect(this->point,
				  elle::Channel::ModeSynchronous) ==
	      elle::StatusError)
	    escape("unable to connect to the bridge");
	}

	leave();
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status	Client::Put(const nucleus::Address&	address,
				    const nucleus::ImmutableBlock& block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(address.component,
							  block);

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Client::Put[Immutable]()"
		    << std::endl;

	// check that the client is connected.
	if (this->gate == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPush>(address,
				    derivable),
	      elle::Outputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to transfer the request");

	leave();
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status	Client::Put(const nucleus::Address&	address,
				    const nucleus::MutableBlock& block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(address.component,
							  block);

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Client::Put[Mutable]()"
		    << std::endl;

	// check that the client is connected.
	if (this->gate == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPush>(address,
				    derivable),
	      elle::Outputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to transfer the request");

	leave();
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status	Client::Get(const nucleus::Address&	address,
				    nucleus::ImmutableBlock&	block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(block);

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Client::Get[Immutable]()"
		    << std::endl;

	// check that the client is connected.
	if (this->gate == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPull>(address,
				    nucleus::Version::Any),
	      elle::Outputs<TagBlock>(derivable)) == elle::StatusError)
	  escape("unable to transfer the request");

	leave();
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status	Client::Get(const nucleus::Address&	address,
				    const nucleus::Version&	version,
				    nucleus::MutableBlock&	block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(block);

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Client::Get[Mutable]()"
		    << std::endl;

	// check that the client is connected.
	if (this->gate == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPull>(address,
				    version),
	      elle::Outputs<TagBlock>(derivable)) == elle::StatusError)
	  escape("unable to transfer the request");

	leave();
      }

      ///
      /// this method removes a block.
      ///
      elle::Status	Client::Kill(const nucleus::Address&	address)
      {
	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Client::Kill()"
		    << std::endl;

	// check that the client is connected.
	if (this->gate == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagWipe>(address),
	      elle::Outputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to transfer the request");

	leave();
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// this callback is triggered whenever the client is challenged
      /// by the server.
      ///
      elle::Status	Client::Challenge()
      {
	enter();

	// debug.
	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Client::Challenge()"
		    << std::endl;

	// return the passport.
	if (this->gate->Reply(
	      elle::Inputs<TagResponse>(Hole::Passport)) == elle::StatusError)
	  escape("unable to return the challenge response");

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Client::Authenticated()
      {
	enter();

	// debug.
	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Client::Authenticated()"
		    << std::endl;

	// this client has succesfully been authenticated, set its state
	// accordingly.
	this->state = Client::StateAuthenticated;

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Client::Error(const elle::Report&	report)
      {
	enter();

	// debug.
	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Client::Error()"
		    << std::endl;

	// transpose the given report.
	transpose(report);

	// report the error.
	report("an error occured on the server side");

	// display the errors before exiting.
	show();

	// exit the program.
	if (elle::Program::Exit() == elle::StatusError)
	  escape("unable to exit the program");

	leave();
      }

      ///
      /// this callback is triggered whenever something unexpected
      /// occurs on the connection.
      ///
      elle::Status	Client::Monitor()
      {
	enter();

	// debug.
	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Client::Monitor()"
		    << std::endl;

	// operate depending on the socket state.
	switch (this->gate->state)
	  {
	  case elle::Channel::StateDisconnected:
	    {
	      // delete the gate.
	      delete this->gate;

	      // reset the gate.
	      this->gate = NULL;

	      // exit if the connection was shut down.
	      if ((this->state == Client::StateConnected) ||
		  (this->state == Client::StateAuthenticated))
		{
		  // exit the program.
		  if (elle::Program::Exit() == elle::StatusError)
		    escape("unable to exit the program");

		  // report the cause.
		  report("the connection with the server has been shut down");

		  // show the report.
		  show();
		}

	      // set the client as unknown.
	      this->state = Client::StateUnknown;

	      break;
	    }
	  case elle::Channel::StateConnected:
	    {
	      // set the client as connected.
	      this->state = Client::StateConnected;

	      break;
	    }
	  default:
	    {
	      // nothing to do.

	      break;
	    }
	  }

	leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the client.
      ///
      elle::Status	Client::Dump(const elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Client]" << std::endl;

	// dump the point.
	if (this->point.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the point");

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
