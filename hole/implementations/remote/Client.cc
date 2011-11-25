//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [thu may 26 10:22:03 2011]
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
      Client::Client(const elle::Locus&				locus):
	state(Client::StateUnknown),
	locus(locus),
	socket(NULL)
      {
      }

      ///
      /// the destructor.
      ///
      Client::~Client()
      {
	// delete the socket.
	if (this->socket != NULL)
	  delete this->socket;
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

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::remote::Client::Launch()\n");

	//
	// register the messages.
	//
	{
	  // register the message.
	  if (elle::Network::Register(
	        elle::Procedure<TagAuthenticated>(
		  elle::Callback<>::Infer(
		    &Client::Authenticated, this))) == elle::StatusError)
	    escape("unable to register the callback");

	  // register the message.
	  if (elle::Network::Register(
	        elle::Procedure<TagException>(
		  elle::Callback<>::Infer(
		    &Client::Exception, this))) == elle::StatusError)
	    escape("unable to register the callback");
	}

	//
	// connect to the server.
	//
	{
	  // allocate the socket.
	  this->socket = new elle::TCPSocket;

	  // create the socket.
	  if (this->socket->Create() == elle::StatusError)
	    escape("unable to create the socket");

	  // subscribe to the signal.
	  if (this->socket->signal.connected.Subscribe(
	        elle::Callback<>::Infer(&Client::Connected,
					this)) == elle::StatusError)
	    escape("unable to subscribe to the signal");

	  // subscribe to the signal.
	  if (this->socket->signal.disconnected.Subscribe(
	        elle::Callback<>::Infer(&Client::Disconnected,
					this)) == elle::StatusError)
	    escape("unable to subscribe to the signal");

	  // subscribe to the signal.
	  if (this->socket->signal.error.Subscribe(
	        elle::Callback<>::Infer(&Client::Error,
					this)) == elle::StatusError)
	    escape("unable to subscribe to the signal");

	  // connect the socket.
	  if (this->socket->Connect(this->locus,
				    elle::StreamSocket::ModeSynchronous) ==
	      elle::StatusError)
	    escape("unable to connect to the bridge");
	}

	//
	// authenticate to the server.
	//
	{
	  // send the passport.
	  if (this->socket->Send(
	        elle::Inputs<TagChallenge>(Hole::Passport)) ==
	      elle::StatusError)
	    escape("unable to send the challenge");
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

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::remote::Client::Put[Immutable]()\n");

	// check that the client is connected.
	if (this->socket == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->socket->Call(
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

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::remote::Client::Put[Mutable]()\n");

	// check that the client is connected.
	if (this->socket == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->socket->Call(
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

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::remote::Client::Get[Immutable]()\n");

	// check that the client is connected.
	if (this->socket == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->socket->Call(
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

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::remote::Client::Get[Mutable]()\n");

	// check that the client is connected.
	if (this->socket == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->socket->Call(
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

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::remote::Client::Kill()\n");

	// check that the client is connected.
	if (this->socket == NULL)
	  escape("the client seems to have been disconnected");

	// transfer to the remote.
	if (this->socket->Call(
	      elle::Inputs<TagWipe>(address),
	      elle::Outputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to transfer the request");

	leave();
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// this callback is triggered whenever the socket is considered
      /// connected.
      ///
      elle::Status	Client::Connected()
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::remote::Client::Connected()\n");

	// set the client as connected.
	this->state = Client::StateConnected;

	leave();
      }

      ///
      /// this callback is triggered whenever the socket is considered
      /// disconnected.
      ///
      elle::Status	Client::Disconnected()
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::remote::Client::Disconnected()\n");

	// exit if the connection was shut down.
	if ((this->state == Client::StateConnected) ||
	    (this->state == Client::StateAuthenticated))
	  {
	    // report the cause.
	    report("the connection with the server has been shut down");

	    // show the report.
	    show();

	    // exit the program.
	    if (elle::Program::Exit() == elle::StatusError)
	      escape("unable to exit the program");
	  }

	leave();
      }

      ///
      /// this callback is triggered whenever an error occurs on the socket.
      ///
      elle::Status	Client::Error(const elle::String&)
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::remote::Client::Error()\n");

	// disconnect the socket, though that may be unecessary.
	this->socket->Disconnect();

	leave();
      }

      ///
      /// this callback is triggered when the Authenticated message is
      /// received from the server meaning that the challenge has been passed.
      ///
      elle::Status	Client::Authenticated()
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::remote::Client::Authenticated()\n");

	// this client has succesfully been authenticated, set its state
	// accordingly.
	this->state = Client::StateAuthenticated;

	leave();
      }

      ///
      /// this callback is triggered whenever the Exception message is
      /// received which indicates the client that an error occured while
      /// processing one of its requests.
      ///
      elle::Status	Client::Exception(const elle::Report&	report)
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::remote::Client::Exception()\n");

	// transpose the given report.
	transpose(report);

	// log the error.
	log("an error occured on the server side");

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

	// dump the locus.
	if (this->locus.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the locus");

	// dump the socket.
	if (this->socket != NULL)
	  {
	    if (this->socket->Dump(margin + 2) == elle::StatusError)
	      escape("unable to dump the socket");
	  }
	else
	  {
	    std::cout << alignment << elle::Dumpable::Shift
		      << "[TCPSocket] " << elle::none << std::endl;
	  }

	leave();
      }

    }
  }
}
