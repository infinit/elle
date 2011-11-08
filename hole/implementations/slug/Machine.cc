//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed aug 31 15:04:40 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/slug/Machine.hh>
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
      /// XXX
      ///
      /// XXX corresponds to 's' i.e 19 and 'l' i.e 12 in the alphabet.
      ///
      const elle::Natural16		Machine::Default::Port = 1912;

      ///
      /// XXX 6 seconds
      ///
      const elle::Natural32		Machine::Timeout = 2000;

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Machine::Machine():
	state(StateUnknown),
	port(0),
	timer(NULL)
      {
      }

      ///
      /// XXX
      ///
      Machine::~Machine()
      {
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
      elle::Status	Machine::Launch()
      {
	elle::Locus	locus;

	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Machine::Launch()\n");

	//
	// register the messages.
	//
	{
	  // register the message.
	  if (elle::Network::Register(
	        elle::Procedure<TagAuthenticate>(
		  elle::Callback<>::Infer(
		    &Machine::Authenticate, this))) == elle::StatusError)
	    escape("unable to register the callback");

	  // register the message.
	  if (elle::Network::Register(
	        elle::Procedure<TagAuthenticated>(
		  elle::Callback<>::Infer(
		    &Machine::Authenticated, this))) == elle::StatusError)
	    escape("unable to register the callback");
	}

	//
	// retrieve information from the descriptor.
	//
	{
	  elle::String	string;

	  // XXX improve this with getting a list of hosts.

	  // retrieve the hosts' loci.
	  if (Hole::Descriptor.Get("slug", "hosts",
				   string) == elle::StatusError)
	    escape("unable to retrieve the slug's host address from the "
		   "network descriptor");

	  // build the host locus.
	  if (locus.Create(string) == elle::StatusError)
	    escape("unable to create the host locus");

	  // retrieve the machine's listening port.
	  if (Hole::Descriptor.Get(
		"slug", "port",
		this->port,
		Machine::Default::Port) == elle::StatusError)
	    escape("unable to retrieve the slug's local port from the "
		   "network descriptor");
	}

	//
	// create the host.
	//
	{
	  Host*		host;

	  // XXX improve this with getting a list of hosts.

	  enterx(instance(host));

	  // allocate the host.
	  host = new Host;

	  // create the host.
	  if (host->Create(locus) == elle::StatusError)
	    escape("unable to create the host");

	  // subscribe to the signal.
	  if (host->signal.dead.Subscribe(
		elle::Callback<>::Infer(&Machine::Sweep,
					this)) == elle::StatusError)
	    escape("unable to subscribe to the signal");

	  // connect the host.
	  if (host->Connect() == elle::StatusError)
	    escape("unable to connect the host");

	  // add the host to the guestlist.
	  if (this->guestlist.Add(host->gate, host) == elle::StatusError)
	    escape("unable to add the host to the guestlist");

	  // waive.
	  waive(host);

	  // release.
	  release();
	}

	//
	// set up the timeout after which the machine will be considered
	// alone in the network, in other words, the very first node.
	//
	{
	  // allocate a timer.
	  this->timer = new elle::Timer;

	  // create the timer.
	  if (this->timer->Create(
		elle::Timer::ModeSingle) == elle::StatusError)
	    escape("unable to create the timer");

	  // subscribe to the timer's signal.
	  if (this->timer->signal.timeout.Subscribe(
		elle::Callback<>::Infer(&Machine::Alone,
					this)) == elle::StatusError)
	    escape("unable to subscribe to the signal");

	  // start the timer.
	  if (this->timer->Start(Machine::Timeout) == elle::StatusError)
	    escape("unable to start the timer");
	}

	//
	// finally, listen for incoming connections.
	//
	{
	  elle::Locus	locus;
	  elle::Host	host;

	  // create a host.
	  if (host.Create(elle::Host::TypeAny) == elle::StatusError)
	    escape("unable to create the host");

	  // create the listening locus.
	  if (locus.Create(host, this->port) == elle::StatusError)
	    escape("unable to create the locus");

	  // listen for incoming connections.
	  if (elle::Bridge::Listen(
		locus,
		elle::Callback<>::Infer(
		  &Machine::Connection, this)) == elle::StatusError)
	    escape("unable to listen for bridge connections");
	}

	leave();
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status	Machine::Put(const nucleus::Address&	address,
				     const nucleus::ImmutableBlock& block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(address.component,
							  block);

	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Machine::Put[Immutable]()\n");

	// depending on the machine's state.
	switch (this->state)
	  {
	  case Machine::StateAlone:
	    {
	      //
	      // in this case, the current machine seems to have been
	      // unable to connect to other hosts.
	      //
	      // therefore, the operation is carried out locally.
	      //

	      // XXX local
	    }
	  case Machine::StateAttached:
	    {
	      //
	      // in this case, the current machine is connected and has
	      // been authenticated as a valid node of the network.
	      //
	      // therefore, the operation is carried out both locally but
	      // also sent to every node in the network.
	      //

	      // XXX local + remote

	/* XXX
	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPush>(address,
				    derivable),
	      elle::Outputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to transfer the request");
	*/

	      break;
	    }
	  default:
	    {
	      escape("the machine's state '%u' does not allow one to request "
		     "operations on the storage layer",
		     this->state);
	    }
	  }

	leave();
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status	Machine::Put(const nucleus::Address&	address,
				     const nucleus::MutableBlock& block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(address.component,
							  block);

	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Machine::Put[Mutable]()\n");

	// depending on the machine's state.
	switch (this->state)
	  {
	  case Machine::StateAlone:
	    {
	      //
	      // in this case, the current machine seems to have been
	      // unable to connect to other hosts.
	      //
	      // therefore, the operation is carried out locally.
	      //

	      // XXX local
	    }
	  case Machine::StateAttached:
	    {
	      //
	      // in this case, the current machine is connected and has
	      // been authenticated as a valid node of the network.
	      //
	      // therefore, the operation is carried out both locally but
	      // also sent to every node in the network.
	      //

	      // XXX local + remote

	/* XXX
	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPush>(address,
				    derivable),
	      elle::Outputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to transfer the request");
	*/

	      break;
	    }
	  default:
	    {
	      escape("the machine's state '%u' does not allow one to request "
		     "operations on the storage layer",
		     this->state);
	    }
	  }

	leave();
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status	Machine::Get(const nucleus::Address&	address,
				     nucleus::ImmutableBlock&	block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(block);

	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Machine::Get[Immutable]()\n");

	// depending on the machine's state.
	switch (this->state)
	  {
	  case Machine::StateAlone:
	    {
	      //
	      // in this case, the current machine seems to have been
	      // unable to connect to other hosts.
	      //
	      // therefore, the operation is carried out locally.
	      //

	      // XXX local
	    }
	  case Machine::StateAttached:
	    {
	      //
	      // in this case, the current machine is connected and has
	      // been authenticated as a valid node of the network.
	      //
	      // therefore, the operation is carried out both locally but
	      // also sent to every node in the network.
	      //

	      // XXX local + remote

	/* XXX
	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPull>(address,
				    nucleus::Version::Any),
	      elle::Outputs<TagBlock>(derivable)) == elle::StatusError)
	  escape("unable to transfer the request");
	*/

	      break;
	    }
	  default:
	    {
	      escape("the machine's state '%u' does not allow one to request "
		     "operations on the storage layer",
		     this->state);
	    }
	  }

	leave();
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status	Machine::Get(const nucleus::Address&	address,
				     const nucleus::Version&	version,
				     nucleus::MutableBlock&	block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(block);

	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Machine::Get[Mutable]()\n");

	// depending on the machine's state.
	switch (this->state)
	  {
	  case Machine::StateAlone:
	    {
	      //
	      // in this case, the current machine seems to have been
	      // unable to connect to other hosts.
	      //
	      // therefore, the operation is carried out locally.
	      //

	      // XXX local
	    }
	  case Machine::StateAttached:
	    {
	      //
	      // in this case, the current machine is connected and has
	      // been authenticated as a valid node of the network.
	      //
	      // therefore, the operation is carried out both locally but
	      // also sent to every node in the network.
	      //

	      // XXX local + remote

	/* XXX
	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPull>(address,
				    version),
	      elle::Outputs<TagBlock>(derivable)) == elle::StatusError)
	  escape("unable to transfer the request");
	*/

	      break;
	    }
	  default:
	    {
	      escape("the machine's state '%u' does not allow one to request "
		     "operations on the storage layer",
		     this->state);
	    }
	  }

	leave();
      }

      ///
      /// this method removes a block.
      ///
      elle::Status	Machine::Kill(const nucleus::Address&	address)
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Machine::Kill()\n");

	// depending on the machine's state.
	switch (this->state)
	  {
	  case Machine::StateAlone:
	    {
	      //
	      // in this case, the current machine seems to have been
	      // unable to connect to other hosts.
	      //
	      // therefore, the operation is carried out locally.
	      //

	      // XXX local
	    }
	  case Machine::StateAttached:
	    {
	      //
	      // in this case, the current machine is connected and has
	      // been authenticated as a valid node of the network.
	      //
	      // therefore, the operation is carried out both locally but
	      // also sent to every node in the network.
	      //

	      // XXX local + remote

	/* XXX
	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagWipe>(address),
	      elle::Outputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to transfer the request");
	*/

	      break;
	    }
	  default:
	    {
	      escape("the machine's state '%u' does not allow one to request "
		     "operations on the storage layer",
		     this->state);
	    }
	  }

	leave();
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status	Machine::Alone()
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Machine::Alone()\n");

	// first, delete the timer.
	bury(timer);

	// reset the pointer.
	this->timer = NULL;

	// if the machine has been neither connected nor authenticated
	// to existing nodes...
	if (this->state == Machine::StateUnknown)
	  {
	    // then, suppose that the current machine as the only one in
	    // the network.
	    //
	    // thus, it can be implicitly considered as authenticated
	    // in a network composed of itself alone.
	    this->state = Machine::StateAlone;
	  }

	leave();
      }

      ///
      /// this method handles new connections.
      ///
      elle::Status	Machine::Connection(elle::Gate*			gate)
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Machine::Connection()\n");

	// depending on the machine's state.
	switch (this->state)
	  {
	  case Machine::StateAlone:
	  case Machine::StateAttached:
	    {
	      Host*		host;

	      enterx(instance(host));

	      // allocate the host.
	      host = new Host;

	      // create the host.
	      if (host->Create(gate) == elle::StatusError)
		escape("unable to create the host");

	      // subscribe to the signal.
	      if (host->signal.dead.Subscribe(
		    elle::Callback<>::Infer(&Machine::Sweep,
					    this)) == elle::StatusError)
		escape("unable to subscribe to the signal");

	      // add the host to the guestlist for now until it
	      // gets authenticated.
	      if (this->guestlist.Add(host->gate, host) == elle::StatusError)
		escape("unable to add the host to the neigbourhood");

	      // also authenticate to this host.
	      if (host->gate->Send(
		    elle::Inputs<TagAuthenticate>(
		      Hole::Passport,
		      this->port)) == elle::StatusError)
		escape("unable to send a message");

	      // waive.
	      waive(host);

	      // release.
	      release();

	      break;
	    }
	  default:
	    {
	      //
	      // if the machine is not authenticated, reject the
	      // incoming connection right away.
	      //

	      // delete the gate.
	      delete gate;

	      break;
	    }
	  }

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Machine::Authenticate(const lune::Passport& passport,
					      const elle::Port&	port)
      {
	Host*		host;
	elle::Session*	session;

	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Machine::Authenticate()\n");

	// retrieve the network session.
	if (elle::Session::Instance(session) == elle::StatusError)
	  escape("unable to retrieve the current session");

	// if the host exists in the guestlist, handle its authentication.
	if (this->guestlist.Exist(
	      static_cast<elle::Gate*>(session->socket)) == elle::StatusTrue)
	  {
	    Cluster	cluster;

	    // retrieve the host from the guestlist.
	    if (this->guestlist.Retrieve(
		  static_cast<elle::Gate*>(session->socket),
		  host) == elle::StatusError)
	      escape("unable to retrieve the host");

	    // create the cluster based on the current neighbourhood.
	    if (cluster.Create(this->neighbourhood) == elle::StatusError)
	      escape("unable to create the cluster");

	    // validate the passport.
	    if (passport.Validate(Infinit::Authority) == elle::StatusError)
	      escape("unable to validate the passport");

	    // create the host's locus according to the given port.
	    if (host->locus.Create(session->locus.host,
				   port) == elle::StatusError)
	      escape("unable to create the locus");

	    // remove the host from the guestlist.
	    if (this->guestlist.Remove(host->gate) == elle::StatusError)
	      escape("unable to remove the host from the guestlist");

	    // add the host to the neighbourhood now that it has been
	    // authenticated.
	    if (this->neighbourhood.Add(host->locus,
					host) == elle::StatusError)
	      escape("unable to add the host to the neighbourhood");

	    // set the host as having been authenticated.
	    if (host->Authenticated() == elle::StatusError)
	      escape("unable to set the host as authenticated");

	    // reply.
	    if (host->gate->Reply(
		  elle::Inputs<TagAuthenticated>(
		    cluster)) == elle::StatusError)
	      escape("unable to reply to the caller");

	    // also authenticate to this host.
	    if (host->gate->Send(
		  elle::Inputs<TagAuthenticate>(
		    Hole::Passport,
		    this->port)) == elle::StatusError)
	      escape("unable to send a message");

	    // XXX
	    this->Dump();
	  }

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Machine::Authenticated(const Cluster&	cluster)
      {
	Host*		host;
	elle::Session*	session;

	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Machine::Authenticated()\n");

	// retrieve the network session.
	if (elle::Session::Instance(session) == elle::StatusError)
	  escape("unable to retrieve the current session");

	// set the machine as being authenticated and is therefore now
	// considered attached to the network.
	this->state = Machine::StateAttached;

	// stop the timer, if present.
	if (this->timer != NULL)
	  {
	    // first, delete the timer.
	    delete timer;

	    // reset the pointer.
	    this->timer = NULL;
	  }

	//
	// use the given cluster to extend the network by connecting to
	// every other host.
	//
	{
	  Cluster::Scoutor	scoutor;

	  // go through the cluster.
	  for (scoutor = cluster.container.begin();
	       scoutor != cluster.container.end();
	       scoutor++)
	    {
	      elle::Locus	locus = *scoutor;
	      Host*		host;

	      enterx(instance(host));

	      // check if this locus is already registered.
	      if (this->neighbourhood.Exist(locus) == elle::StatusTrue)
		continue;

	      // allocate the host.
	      host = new Host;

	      // create the host.
	      if (host->Create(locus) == elle::StatusError)
		escape("unable to create the host");

	      // subscribe to the signal.
	      if (host->signal.dead.Subscribe(
		    elle::Callback<>::Infer(&Machine::Sweep,
					    this)) == elle::StatusError)
		escape("unable to subscribe to the signal");

	      // connect the host.
	      if (host->Connect() == elle::StatusError)
		escape("unable to connect the host");

	      // add the host to the guestlist.
	      if (this->guestlist.Add(host->gate, host) == elle::StatusError)
		escape("unable to add the host to the guestlist");

	      // waive.
	      waive(host);

	      // release.
	      release();
	    }
	}

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Machine::Sweep(Host*			host)
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Machine::Sweep()\n");

	// depending on the host's state.
	switch (host->state)
	  {
	  case Host::StateAuthenticated:
	    {
	      // remove the host from the neighbourhood.
	      if (this->neighbourhood.Remove(host->locus) == elle::StatusError)
		escape("unable to remove the host from the neighbourhood");

	      break;
	    }
	  default:
	    {
	      // does the host exist in the guestlist.
	      if (this->guestlist.Exist(host->gate) == elle::StatusTrue)
		{
		  // remove the host from the guestlist.
		  if (this->guestlist.Remove(host->gate) == elle::StatusError)
		    escape("unable to remove the host from the neighbourhood");
		}

	      break;
	    }
	  }

	// delete the host.
	bury(host);
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the machine.
      ///
      elle::Status	Machine::Dump(const elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Machine]" << std::endl;

	// dump the state.
	std::cout << alignment << elle::Dumpable::Shift
		  << "[State] " << this->state << std::endl;

	// dump the port.
	std::cout << alignment << elle::Dumpable::Shift
		  << "[Port] " << std::dec << this->port << std::endl;

	// dump the guestlist.
	if (this->guestlist.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the guestlist");

	// dump the neighbourhood.
	if (this->neighbourhood.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the neighbourhood");

	leave();
      }

    }
  }
}
