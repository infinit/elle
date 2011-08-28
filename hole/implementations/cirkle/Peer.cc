//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/cirkle/Peer.cc
//
// created       julien quintard   [thu may 26 10:22:03 2011]
// updated       julien quintard   [sun aug 28 18:07:51 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/cirkle/Peer.hh>
#include <hole/implementations/cirkle/Manifest.hh>
#include <hole/implementations/cirkle/Neighbour.hh>
#include <hole/Hole.hh>

#include <Infinit.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Peer::Peer():
	state(StateUnauthenticated)
      {
      }

//
// ---------- peer ------------------------------------------------------------
//

      ///
      /// this method initializes the peer.
      ///
      elle::Status	Peer::Initialize(const elle::Port	port,
					 const elle::Point&	point)
      {
	enter();

	//
	// register the messages.
	//
	{
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<>
	    >				challenge(&Peer::Challenge, this);
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<
	      const lune::Passport
	      >
	    >				authenticate(&Peer::Authenticate,
						     this);
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<>
	    >				authenticated(&Peer::Authenticated,
						      this);
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<>
	    >				listen(&Peer::Listen, this);
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<
	      const elle::Port
	      >
	    >				port(&Peer::Port, this);
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<>
	    >				request(&Peer::Request, this);
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<
	      const Cluster
	      >
	    >				gossip(&Peer::Gossip, this);

	  // register the message.
	  if (elle::Network::Register(
	        elle::Procedure<TagChallenge>(challenge)) == elle::StatusError)
	    escape("unable to register the callback");

	  // register the message.
	  if (elle::Network::Register(
	        elle::Procedure<TagAuthenticate>(authenticate)) ==
	      elle::StatusError)
	    escape("unable to register the callback");

	  // register the message.
	  if (elle::Network::Register(
	        elle::Procedure<TagAuthenticated>(authenticated)) ==
	      elle::StatusError)
	    escape("unable to register the callback");

	  // register the message.
	  if (elle::Network::Register(
	        elle::Procedure<TagListen>(listen)) ==
	      elle::StatusError)
	    escape("unable to register the callback");

	  // register the message.
	  if (elle::Network::Register(
	        elle::Procedure<TagPort>(port)) ==
	      elle::StatusError)
	    escape("unable to register the callback");

	  // register the message.
	  if (elle::Network::Register(
	        elle::Procedure<TagRequest>(request)) == elle::StatusError)
	    escape("unable to register the callback");

	  // register the message.
	  if (elle::Network::Register(
	        elle::Procedure<TagGossip>(gossip)) == elle::StatusError)
	    escape("unable to register the callback");
	}

	//
	// create the neighbour.
	//
	{
	  Neighbour*	neighbour;

	  enter(instance(neighbour));

	  // allocate the neighbour.
	  neighbour = new Neighbour;

	  // create the neighbour.
	  if (neighbour->Create(point) == elle::StatusError)
	    escape("unable to create the neighbour");

	  // add the neighbour to the neighbourhood.
	  if (this->neighbourhood.Add(neighbour->point, neighbour) ==
	      elle::StatusError)
	    escape("unable to add the neighbour to the neighbourhood");

	  // connect the neighbour.
	  if (neighbour->Connect() == elle::StatusError)
	    escape("unable to connect the neighbour");

	  // waive.
	  waive(neighbour);

	  // release.
	  release();
	}

	//
	// listen for incoming connections
	//
	{
	  elle::Callback<
	    elle::Status,
	    elle::Parameters<
	      elle::Gate*
	      >
	    >				connection(&Peer::Connection, this);
	  elle::Point			point;
	  elle::Host			host;

	  // create a host.
	  // XXX ???
	  if (host.Create("127.0.0.1") == elle::StatusError)
	    escape("unable to create the host");

	  // create the listening point.
	  if (point.Create(host, port) == elle::StatusError)
	    escape("unable to create the point");

	  // listen for incoming connections.
	  if (elle::Bridge::Listen(point, connection) == elle::StatusError)
	    escape("unable to listen for bridge connections");
	}

	leave();
      }

      ///
      /// this method cleans the peer.
      ///
      elle::Status	Peer::Clean()
      {
	enter();

	// nothing to do.

	leave();
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status	Peer::Put(const nucleus::Address&	address,
				  const nucleus::ImmutableBlock& block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(address.component,
							  block);

	enter();
	/* XXX
	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Peer::Put[Immutable]()"
		    << std::endl;

	// check that the peer is connected.
	if (this->gate == NULL)
	  escape("the peer seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPush>(address,
				    derivable),
	      elle::Outputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to transfer the request");
	*/
	leave();
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status	Peer::Put(const nucleus::Address&	address,
				  const nucleus::MutableBlock&	block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(address.component,
							  block);

	enter();
	/* XXX
	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Peer::Put[Mutable]()"
		    << std::endl;

	// check that the peer is connected.
	if (this->gate == NULL)
	  escape("the peer seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPush>(address,
				    derivable),
	      elle::Outputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to transfer the request");
	*/
	leave();
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status	Peer::Get(const nucleus::Address&	address,
				  nucleus::ImmutableBlock&	block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(block);

	enter();
	/* XXX
	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Peer::Get[Immutable]()"
		    << std::endl;

	// check that the peer is connected.
	if (this->gate == NULL)
	  escape("the peer seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPull>(address,
				    nucleus::Version::Any),
	      elle::Outputs<TagBlock>(derivable)) == elle::StatusError)
	  escape("unable to transfer the request");
	*/
	leave();
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status	Peer::Get(const nucleus::Address&	address,
				  const nucleus::Version&	version,
				  nucleus::MutableBlock&	block)
      {
	nucleus::Derivable<nucleus::Block>	derivable(block);

	enter();
	/* XXX
	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Peer::Get[Mutable]()"
		    << std::endl;

	// check that the peer is connected.
	if (this->gate == NULL)
	  escape("the peer seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagPull>(address,
				    version),
	      elle::Outputs<TagBlock>(derivable)) == elle::StatusError)
	  escape("unable to transfer the request");
	*/
	leave();
      }

      ///
      /// this method removes a block.
      ///
      elle::Status	Peer::Kill(const nucleus::Address&	address)
      {
	enter();
	/* XXX
	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Peer::Kill()"
		    << std::endl;

	// check that the peer is connected.
	if (this->gate == NULL)
	  escape("the peer seems to have been disconnected");

	// transfer to the remote.
	if (this->gate->Call(
	      elle::Inputs<TagWipe>(address),
	      elle::Outputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to transfer the request");
	*/
	leave();
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// this method handles new connections.
      ///
      elle::Status	Peer::Connection(elle::Gate*&		gate)
      {
	Neighbour*	neighbour;

	enter(instance(neighbour));

	printf("CONNECTION\n");

	// allocate the neighbour.
	neighbour = new Neighbour;

	// create the neighbour.
	if (neighbour->Create(gate) == elle::StatusError)
	  escape("unable to create the neighbour");

	// add the neighbour to the neighbourhood.
	if (this->neighbourhood.Add(neighbour->point, neighbour) ==
	    elle::StatusError)
	  escape("unable to add the neighbour to the neighbourhood");

	// challenge the peer.
	if (neighbour->gate->Send(
	      elle::Inputs<TagChallenge>()) == elle::StatusError)
	  escape("unable to send the challenge");

	// waive.
	waive(neighbour);

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Peer::Challenge()
      {
	Neighbour*	neighbour;
	elle::Session*	session;

	enter();

	printf("CHALLENGE\n");

	// retrieve the network session.
	if (elle::Session::Instance(session) == elle::StatusError)
	  escape("unable to retrieve the current session");

	// retrieve the neighbour from the neighbourhood.
	if (this->neighbourhood.Retrieve(session->point,
					 neighbour) == elle::StatusError)
	  escape("unable to retrieve the neighbour");

	// send the passport.
	if (neighbour->gate->Send(
	      elle::Inputs<TagAuthenticate>(Hole::Passport)) ==
	    elle::StatusError)
	  escape("unable to authenticate");

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Peer::Authenticate(const lune::Passport& passport)
      {
	Neighbour*	neighbour;
	elle::Session*	session;

	enter();

	printf("AUTHENTICATE\n");

	// retrieve the network session.
	if (elle::Session::Instance(session) == elle::StatusError)
	  escape("unable to retrieve the current session");

	// retrieve the neighbour from the neighbourhood.
	if (this->neighbourhood.Retrieve(session->point,
					 neighbour) == elle::StatusError)
	  escape("unable to retrieve the neighbour");

	// check the neighbour's state, ignore 
	if (neighbour->state == Neighbour::StateAuthenticated)
	  leave();

	// validate the passport.
	if (passport.Validate(Infinit::Authority) == elle::StatusError)
	  escape("unable to validate the passport");

	// set the neighbour as authenticated.
	neighbour->state = Neighbour::StateAuthenticated;

	// set the neighbour's label.
	neighbour->label = passport.label;

	// register the neighbour in the routing table.
	if (this->table.Add(neighbour->label, neighbour) == elle::StatusError)
	  escape("unable to add the neighbour to the routing table");

	// acknowledge the authentication.
	if (neighbour->gate->Reply(
	      elle::Inputs<TagAuthenticated>()) == elle::StatusError)
	  escape("unable to acknowledge the authentication");

	// XXX
	Cirkle::Machine.Dump();

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Peer::Authenticated()
      {
	Neighbour*	neighbour;
	elle::Session*	session;

	enter();

	printf("AUTHENTICATED\n");

	// retrieve the network session.
	if (elle::Session::Instance(session) == elle::StatusError)
	  escape("unable to retrieve the current session");

	// retrieve the neighbour from the neighbourhood.
	if (this->neighbourhood.Retrieve(session->point,
					 neighbour) == elle::StatusError)
	  escape("unable to retrieve the neighbour");

	// if the machine has not retrieve the cirkle's members yet, do it.
	if (this->state == Peer::StateUnauthenticated)
	  {
	    // request the cluster of peers.
	    if (neighbour->gate->Send(
		  elle::Inputs<TagRequest>()) ==
		elle::StatusError)
	      escape("unable to request the cluster of peers");

	    // set the state.
	    this->state = Peer::StateAuthenticated;
	  }

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Peer::Listen()
      {
	Neighbour*	neighbour;
	elle::Session*	session;

	enter();

	printf("LISTEN\n");

	// retrieve the network session.
	if (elle::Session::Instance(session) == elle::StatusError)
	  escape("unable to retrieve the current session");

	// retrieve the neighbour from the neighbourhood.
	if (this->neighbourhood.Retrieve(session->point,
					 neighbour) == elle::StatusError)
	  escape("unable to retrieve the neighbour");

	// XXX retrieve currently listening port.

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Peer::Port(const elle::Port&		port)
      {
	Neighbour*	neighbour;
	elle::Session*	session;

	enter();

	printf("POINT\n");

	// retrieve the network session.
	if (elle::Session::Instance(session) == elle::StatusError)
	  escape("unable to retrieve the current session");

	// retrieve the neighbour from the neighbourhood.
	if (this->neighbourhood.Retrieve(session->point,
					 neighbour) == elle::StatusError)
	  escape("unable to retrieve the neighbour");

	// update the neighbour's listening point.
	// XXX clone gate's target with this port.

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Peer::Request()
      {
	Neighbour*	neighbour;
	elle::Session*	session;
	Cluster		cluster;

	enter();

	printf("REQUEST\n");

	// retrieve the network session.
	if (elle::Session::Instance(session) == elle::StatusError)
	  escape("unable to retrieve the current session");

	// retrieve the neighbour from the neighbourhood.
	if (this->neighbourhood.Retrieve(session->point,
					 neighbour) == elle::StatusError)
	  escape("unable to retrieve the neighbour");

	// check that the neighbour has been authenticated.
	if (neighbour->state != Neighbour::StateAuthenticated)
	  escape("unable to deal with unauthenticated peers");

	/// XXX create from routing table.
	// create the cluster.
	if (cluster.Create(this->neighbourhood) == elle::StatusError)
	  escape("unable to create the cluster");

	// return the cluster.
	if (neighbour->gate->Reply(
	      elle::Inputs<TagGossip>(cluster)) ==
	    elle::StatusError)
	  escape("unable to return the cluster of peers peers");

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Peer::Gossip(const Cluster&		cluster)
      {
	Neighbour*		neighbour;
	elle::Session*		session;
	Cluster::Scoutor	scoutor;

	enter();

	printf("GOSSIP\n");

	// retrieve the network session.
	if (elle::Session::Instance(session) == elle::StatusError)
	  escape("unable to retrieve the current session");

	// retrieve the neighbour from the neighbourhood.
	if (this->neighbourhood.Retrieve(session->point,
					 neighbour) == elle::StatusError)
	  escape("unable to retrieve the neighbour");

	// check that the neighbour has been authenticated.
	if (neighbour->state != Neighbour::StateAuthenticated)
	  escape("unable to deal with unauthenticated peers");

	// go through the cluster.
	for (scoutor = cluster.container.begin();
	     scoutor != cluster.container.end();
	     scoutor++)
	  {
	    Cluster::Atom	atom = *scoutor;
	    Neighbour*		neighbour;

	    /* XXX
	    enter(instance(neighbour));

	    // check if this point is already registered.
	    if (this->neighbourhood.Exist(atom.label) == elle::StatusTrue)
	      continue;

	    printf("NEW POINT\n");
	    scoutor->Dump();

	    // allocate the neighbour.
	    neighbour = new Neighbour;

	    // create the neighbour.
	    if (neighbour->Create(*scoutor) == elle::StatusError)
	      escape("unable to create the neighbour");

	    // add the neighbour to the neighbourhood.
	    if (this->neighbourhood.Add(neighbour->point, neighbour) ==
		elle::StatusError)
	      escape("unable to add the neighbour to the neighbourhood");

	    // connect the neighbour.
	    if (neighbour->Connect() == elle::StatusError)
	      escape("unable to connect the neighbour");

	    // waive.
	    waive(neighbour);

	    // release.
	    release();
	    */
	  }

	leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the peer.
      ///
      elle::Status	Peer::Dump(const elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Peer]" << std::endl;

	// dump the neighbourhood.
	if (this->neighbourhood.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the neighbourhood");

	// dump the routing table.
	if (this->table.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the routing table");

	leave();
      }

    }
  }
}
