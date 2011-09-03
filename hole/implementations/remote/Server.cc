//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Server.cc
//
// created       julien quintard   [thu may 26 09:58:52 2011]
// updated       julien quintard   [sat sep  3 09:49:14 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Server.hh>
#include <hole/implementations/remote/Manifest.hh>

#include <hole/Hole.hh>

#include <lune/Lune.hh>

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
      Server::Server(const elle::Point&				point):
	point(point)
      {
      }

      ///
      /// destructor.
      ///
      Server::~Server()
      {
	Server::Scoutor	scoutor;

	// go though the customer.
	for (scoutor = this->container.begin();
	     scoutor != this->container.end();
	     scoutor++)
	  {
	    Customer*	customer = scoutor->second;

	    // delete the customer.
	    delete customer;
	  }

	// clear the container.
	this->container.clear();
      }

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// launch the server by waiting for incoming connections.
      ///
      elle::Status	Server::Launch()
      {
	enter();

	//
	// register the messages.
	//
	{
	  // register the response message.
	  if (elle::Network::Register(
	        elle::Procedure<TagResponse,
				elle::TagNone,
				TagException>(
		  elle::Callback<>::Infer(
		    &Server::Response, this))) == elle::StatusError)
	    escape("unable to register the callback");

	  // register the push message.
	  if (elle::Network::Register(
	        elle::Procedure<TagPush,
				elle::TagNone,
				TagException>(
		  elle::Callback<>::Infer(
		    &Server::Push, this))) == elle::StatusError)
	    escape("unable to register the callback");

	  // register the pull message.
	  if (elle::Network::Register(
	        elle::Procedure<TagPull,
				elle::TagNone,
				TagException>(
		  elle::Callback<>::Infer(
		    &Server::Pull, this))) == elle::StatusError)
	    escape("unable to register the callback");

	  // register the wipe message.
	  if (elle::Network::Register(
	        elle::Procedure<TagWipe,
				elle::TagNone,
				TagException>(
		  elle::Callback<>::Infer(
		    &Server::Wipe, this))) == elle::StatusError)
	    escape("unable to register the callback");
	}

	//
	// create the connection.
	//
	{
	  // listen for incoming connections.
	  if (elle::Bridge::Listen(
		this->point,
	        elle::Callback<>::Infer(
		  &Server::Connection, this)) == elle::StatusError)
	    escape("unable to listen for bridge connections");
	}

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Server::Add(elle::Gate*			gate,
				    Customer*			customer)
      {
	std::pair<Server::Iterator, elle::Boolean>	result;

	enter();

	// check if this customer already exists.
	if (this->Locate(gate) == elle::StatusTrue)
	  escape("this gate has already been registered");

	// insert the customer in the container.
	result = this->container.insert(
		   std::pair<elle::Gate*, Customer*>(gate, customer));

	// check if the insertion was successful.
	if (result.second == false)
	  escape("unable to insert the customer in the container");

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Server::Remove(elle::Gate*		gate)
      {
	Server::Iterator	iterator;

	enter();

	// locate the customer.
	if (this->Locate(gate, &iterator) == elle::StatusFalse)
	  escape("unable to locate the given customer");

	// remove the entry from the container.
	this->container.erase(iterator);

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Server::Retrieve(elle::Gate*		gate,
					 Customer*&		customer)
      {
	Server::Iterator	iterator;

	enter();

	// locate the customer.
	if (this->Locate(gate, &iterator) == elle::StatusFalse)
	  escape("unable to locate the given customer");

	// retrieve the customer.
	customer = iterator->second;

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Server::Locate(elle::Gate*		gate,
				       Iterator*		iterator)
      {
	Server::Iterator	i;

	enter();

	// try to locate the customer.
	if ((i = this->container.find(gate)) != this->container.end())
	  {
	    if (iterator != NULL)
	      *iterator = i;

	    true();
	  }

	false();
      }

      ///
      /// this method stores an immutable block.
      ///
      elle::Status	Server::Put(const nucleus::Address&	address,
				    const nucleus::ImmutableBlock& block)
      {
	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Put[Immutable]()"
		    << std::endl;

	// does the block already exist.
	if (block.Exist(Hole::Implementation->network,
			address) == elle::StatusTrue)
	  escape("this immutable block seems to already exist");

	// store the block.
	if (block.Store(Hole::Implementation->network,
			address) == elle::StatusError)
	  escape("unable to store the block");

	leave();
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status	Server::Put(const nucleus::Address&	address,
				    const nucleus::MutableBlock& block)
      {
	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Put[Mutable]()"
		    << std::endl;

	// does the block already exist.
	if (block.Exist(Hole::Implementation->network,
			address,
			nucleus::Version::Last) == elle::StatusTrue)
	  {
	    nucleus::MutableBlock*	current;

	    // build a block according to the component.
	    if (nucleus::Nucleus::Factory.Build(address.component,
						current) == elle::StatusError)
	      escape("unable to build the block");

	    // load the latest version.
	    if (current->Load(Hole::Implementation->network,
			      address,
			      nucleus::Version::Last) == elle::StatusError)
	      escape("unable to load the current version");

	    // does the given block derive the current version.
	    if (!(block.version > current->version))
	      escape("the block to store does not seem to derive the current "
		     "version");

	    // delete the current instance.
	    delete current;

	    // waive.
	    waive(current);

	    // release.
	    release();
	  }

	// store the block.
	if (block.Store(Hole::Implementation->network,
			address) == elle::StatusError)
	  escape("unable to store the block");

	leave();
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status	Server::Get(const nucleus::Address&	address,
				    nucleus::ImmutableBlock&	block)
      {
	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Get[Immutable]()"
		    << std::endl;

	// does the block exist.
	if (block.Exist(Hole::Implementation->network,
			address) == elle::StatusFalse)
	  escape("the block does not seem to exist");

	// load the block.
	if (block.Load(Hole::Implementation->network,
		       address) == elle::StatusError)
	  escape("unable to load the block");

	// validate the block.
	if (block.Validate(address) == elle::StatusError)
	  escape("the block seems to be invalid");

	leave();
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status	Server::Get(const nucleus::Address&	address,
				    const nucleus::Version&	version,
				    nucleus::MutableBlock&	block)
      {
	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Get[Mutable]()"
		    << std::endl;

	// does the block exist.
	if (block.Exist(Hole::Implementation->network,
			address, version) == elle::StatusFalse)
	  escape("the block does not seem to exist");

	// load the block.
	if (block.Load(Hole::Implementation->network,
		       address, version) == elle::StatusError)
	  escape("unable to load the block");

	// validate the block.
	if (block.Validate(address) == elle::StatusError)
	  escape("the block seems to be invalid");

	leave();
      }

      ///
      /// this method removes a block.
      ///
      elle::Status	Server::Kill(const nucleus::Address&	address)
      {
	nucleus::Block	block;

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Kill()"
		    << std::endl;

	// treat the request depending on the nature of the block which
	// the addres indicates.
	switch (address.family)
	  {
	  case nucleus::FamilyContentHashBlock:
	    {
	      nucleus::ImmutableBlock	ib;

	      // erase the immutable block.
	      if (ib.Erase(Hole::Implementation->network,
			   address) == elle::StatusError)
		escape("unable to erase the block");

	      break;
	    }
	  case nucleus::FamilyPublicKeyBlock:
	  case nucleus::FamilyOwnerKeyBlock:
	  case nucleus::FamilyImprintBlock:
	    {
	      nucleus::MutableBlock	mb;

	      // retrieve the mutable block.
	      if (mb.Erase(Hole::Implementation->network,
			   address) == elle::StatusError)
		escape("unable to erase the block");

	      break;
	    }
	  default:
	    {
	      escape("unknown block family");
	    }
	  }

	leave();
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// this callback handles new connections.
      ///
      elle::Status	Server::Connection(elle::Gate*		gate)
      {
	Customer*	customer;

	enter(instance(customer));

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Connection()"
		    << std::endl;

	// allocate a customer.
	customer = new Customer;

	// create the customer.
	if (customer->Create(gate) == elle::StatusError)
	  escape("unable to create the customer");

	// set the state.
	customer->state = Customer::StateConnected;

	// add the customer.
	if (this->Add(customer->gate, customer) == elle::StatusError)
	  escape("unable to add the customer");

	// send a message to the client in order to challenge it.
	if (customer->gate->Send(
	      elle::Inputs<TagChallenge>()) == elle::StatusError)
	  escape("unable to send a message to the client");

	// waive.
	waive(customer);

	leave();
      }

      ///
      /// this callback is triggered whenever the client responds to
      /// the initial challenge.
      ///
      elle::Status	Server::Response(const lune::Passport&	passport)
      {
	Customer*	customer;
	elle::Session*	session;

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Response()"
		    << std::endl;

	// retrieve the network session.
	if (elle::Session::Instance(session) == elle::StatusError)
	  escape("unable to retrieve the current session");

	// retrieve the customer.
	if (this->Retrieve(dynamic_cast<elle::Gate*>(session->socket),
			   customer) == elle::StatusError)
	  escape("unable to retrieve the customer");

	// validate the passport.
	if (passport.Validate(Infinit::Authority) == elle::StatusError)
	  {
	    // remove the customer.
	    if (this->Remove(customer->gate) == elle::StatusError)
	      escape("unable to remove the customer");

	    // delete the customer.
	    delete customer;
	  }
	else
	  {
	    // set the state as authenticated.
	    customer->state = Customer::StateAuthenticated;

	    // reply with the authenticated message.
	    if (customer->gate->Reply(
	          elle::Inputs<TagAuthenticated>()) == elle::StatusError)
	      escape("unable to reply to the client");
	  }

	leave();
      }

      ///
      /// this method stores the given block.
      ///
      elle::Status	Server::Push(const nucleus::Address&	address,
				     const
				       nucleus::Derivable
				         <nucleus::Block>&	derivable)
      {
	Customer*	customer;
	elle::Session*	session;
	nucleus::Block*	object;

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Push()"
		    << std::endl;

	// retrieve the network session.
	if (elle::Session::Instance(session) == elle::StatusError)
	  escape("unable to retrieve the current session");

	// retrieve the customer.
	if (this->Retrieve(dynamic_cast<elle::Gate*>(session->socket),
			   customer) == elle::StatusError)
	  escape("unable to retrieve the customer");

	// check that the client has been authenticated.
	if (customer->state != Customer::StateAuthenticated)
	  escape("the customer has not been authenticated");

	// infer the block from the derivable.
	if (derivable.Infer(object) == elle::StatusError)
	  escape("unable to infer the block from the derivable");

	// forward the request depending on the nature of the block which
	// the addres indicates.
	switch (address.family)
	  {
	  case nucleus::FamilyContentHashBlock:
	    {
	      nucleus::ImmutableBlock*	ib;

	      // cast to an immutable block.
	      ib = static_cast<nucleus::ImmutableBlock*>(object);

	      // store the immutable block.
	      if (this->Put(address, *ib) == elle::StatusError)
		escape("unable to put the block");

	      break;
	    }
	  case nucleus::FamilyPublicKeyBlock:
	  case nucleus::FamilyOwnerKeyBlock:
	  case nucleus::FamilyImprintBlock:
	    {
	      nucleus::MutableBlock*	mb;

	      // cast to a mutable block.
	      mb = static_cast<nucleus::MutableBlock*>(object);

	      // store the mutable block.
	      if (this->Put(address, *mb) == elle::StatusError)
		escape("unable to put the block");

	      break;
	    }
	  default:
	    {
	      escape("unknown block family");
	    }
	  }

	// acknowledge.
	if (customer->gate->Reply(
	      elle::Inputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to acknowledge");

	leave();
      }

      ///
      /// this method returns the block associated with the given address.
      ///
      elle::Status	Server::Pull(const nucleus::Address&	address,
				     const nucleus::Version&	version)
      {
	Customer*	customer;
	elle::Session*	session;
	nucleus::Block*	block;

	enter(instance(block));

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Pull()"
		    << std::endl;

	// retrieve the network session.
	if (elle::Session::Instance(session) == elle::StatusError)
	  escape("unable to retrieve the current session");

	// retrieve the customer.
	if (this->Retrieve(dynamic_cast<elle::Gate*>(session->socket),
			   customer) == elle::StatusError)
	  escape("unable to retrieve the customer");

	// check that the client has been authenticated.
	if (customer->state != Customer::StateAuthenticated)
	  escape("the customer has not been authenticated");

	// build the block according to the component.
	if (nucleus::Nucleus::Factory.Build(address.component,
					    block) == elle::StatusError)
	  escape("unable to build the block");

	// forward the request depending on the nature of the block which
	// the addres indicates.
	switch (address.family)
	  {
	  case nucleus::FamilyContentHashBlock:
	    {
	      nucleus::ImmutableBlock*	ib;

	      // cast to an immutable block.
	      ib = static_cast<nucleus::ImmutableBlock*>(block);

	      // retrieve the immutable block.
	      if (this->Get(address, *ib) == elle::StatusError)
		escape("unable to get the block");

	      break;
	    }
	  case nucleus::FamilyPublicKeyBlock:
	  case nucleus::FamilyOwnerKeyBlock:
	  case nucleus::FamilyImprintBlock:
	    {
	      nucleus::MutableBlock*	mb;

	      // cast to a mutable block.
	      mb = static_cast<nucleus::MutableBlock*>(block);

	      // retrieve the mutable block.
	      if (this->Get(address, version,
			    *mb) == elle::StatusError)
		escape("unable to get the block");

	      break;
	    }
	  default:
	    {
	      escape("unknown block family");
	    }
	  }

	nucleus::Derivable<nucleus::Block>	derivable(address.component,
							  *block);

	// return the block.
	if (customer->gate->Reply(
	      elle::Inputs<TagBlock>(derivable)) == elle::StatusError)
	  escape("unable to return the block");

	// delete the block.
	delete block;

	// waive.
	waive(block);

	leave();
      }

      ///
      /// this method removes the block associated with the given address.
      ///
      elle::Status	Server::Wipe(const nucleus::Address&	address)
      {
	Customer*	customer;
	elle::Session*	session;

	enter();

	if (Infinit::Configuration.debug.hole == true)
	  std::cout << "[hole] Server::Wipe()"
		    << std::endl;

	// retrieve the network session.
	if (elle::Session::Instance(session) == elle::StatusError)
	  escape("unable to retrieve the current session");

	// retrieve the customer.
	if (this->Retrieve(dynamic_cast<elle::Gate*>(session->socket),
			   customer) == elle::StatusError)
	  escape("unable to retrieve the customer");

	// check that the client has been authenticated.
	if (customer->state != Customer::StateAuthenticated)
	  escape("the customer has not been authenticated");

	// forward the kill request to the implementation.
	if (this->Kill(address) == elle::StatusError)
	  escape("unable to erase the block");

	// acknowledge.
	if (customer->gate->Reply(
	      elle::Inputs<elle::TagOk>()) == elle::StatusError)
	  escape("unable to acknowledge");

	leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the server.
      ///
      elle::Status	Server::Dump(const elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');
	Server::Scoutor	scoutor;

	enter();

	std::cout << alignment << "[Server]" << std::endl;

	// dump the point.
	if (this->point.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the point");

	std::cout << alignment << elle::Dumpable::Shift
		  << "[Customers]" << std::endl;

	// go though the customer.
	for (scoutor = this->container.begin();
	     scoutor != this->container.end();
	     scoutor++)
	  {
	    Customer*	customer = scoutor->second;

	    // dump the customer.
	    if (customer->Dump(margin + 4) == elle::StatusError)
	      escape("unable to dump the customer");
	  }

	leave();
      }

    }
  }
}
