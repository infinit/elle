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
// updated       julien quintard   [fri aug 26 19:10:32 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/cirkle/Peer.hh>
#include <hole/implementations/cirkle/Manifest.hh>
#include <hole/implementations/cirkle/Neighbour.hh>

#include <Infinit.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

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
	    elle::Parameters<
	      const Cluster
	      >
	    >				update(&Peer::Update, this);

	  // register the challenge message.
	  if (elle::Network::Register(
	        elle::Procedure<TagGossip>(update)) == elle::StatusError)
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

      ///
      /// XXX
      ///
      elle::Status	Peer::Propagate(Neighbour*		neighbour)
      {
	Cluster		cluster;

	enter();

	printf("[XXX] Peer::Propagate()\n");

	// generate a cluster from the current routing table.
	if (cluster.Create(this->table) == elle::StatusError)
	  escape("unable to create the cluster");

	// send the cluster.
	if (neighbour->gate->Send(
	      elle::Inputs<TagGossip>(cluster)) == elle::StatusError)
	  escape("unable to send the cluster");

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

	// allocate the neighbour.
	neighbour = new Neighbour;

	// create the neighbour.
	if (neighbour->Create(gate) == elle::StatusError)
	  escape("unable to create the neighbour");

	// add the neighbour to the neighbourhood.
	if (this->neighbourhood.Add(neighbour->point, neighbour) ==
	    elle::StatusError)
	  escape("unable to add the neighbour to the neighbourhood");

	// challenge the neighbour.
	if (neighbour->Challenge() == elle::StatusError)
	  escape("unable to challenge the neighbour");

	// waive.
	waive(neighbour);

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Peer::Update(const Cluster&		cluster)
      {
	elle::Session*	session;

	enter();

	printf("[XXX] Peer::Update()\n");

	elle::Session::Instance(session);

	session->Dump();

	// XXX recuperer la socket dans Session.

	// XXX retrouver l'entree dans la RoutingTable
	// -> si aucune, on rejete
	// -> sinon on update la notre

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
