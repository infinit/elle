//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/Hole.cc
//
// created       julien quintard   [tue apr 13 15:27:20 2010]
// updated       julien quintard   [fri may 13 15:27:42 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Hole.hh>

#include <hole/Manifest.hh>

#include <lune/Lune.hh>

namespace hole
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "hole";

  ///
  /// this defines the name of the door which will be used by clients.
  ///
  const elle::String&		Hole::Line("hole");

  ///
  /// this variables contains all the networks Hole is currently connected
  /// to.
  ///
  Hole::Container		Hole::Networks;

  ///
  /// this variable holds the channel object connecting Etoile to Hole.
  ///
  elle::Channel*		Hole::Channel = NULL;

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method initializes the hole by allocating and initializing
  /// the implementation.
  ///
  elle::Status		Hole::Initialize()
  {
    enter();

    //
    // register the messages.
    //
    {
      elle::Callback<const elle::String>	error(&Hole::Error);
      elle::Callback<const nucleus::Network>	join(&Hole::Join);
      elle::Callback<const nucleus::Network>	leav(&Hole::Leave);
      elle::Callback<const nucleus::Network,
		     const nucleus::Address,
		     const elle::Derivable
		       <nucleus::Block> >	put(&Hole::Put);
      elle::Callback<const nucleus::Network,
		     const nucleus::Address>	get(&Hole::Get);
      elle::Callback<const nucleus::Network,
		     const nucleus::Address>	erase(&Hole::Erase);

      // register the join message.
      if (elle::Network::Register<TagJoin>(join) == elle::StatusError)
	escape("unable to register the callback");

      // register the leave message.
      if (elle::Network::Register<TagLeave>(leav) == elle::StatusError)
	escape("unable to register the callback");

      // register the put message.
      if (elle::Network::Register<TagPut>(put) == elle::StatusError)
	escape("unable to register the callback");

      // register the get message.
      if (elle::Network::Register<TagGet>(get) == elle::StatusError)
	escape("unable to register the callback");

      // register the erase message.
      if (elle::Network::Register<TagErase>(erase) == elle::StatusError)
	escape("unable to register the callback");
    }

    //
    // create the connection.
    //
    {
      elle::Callback<elle::Door*>		connection(&Hole::Connection);

      /// XXX \todo le faire proprement en quittant, tout nettoyer, notamment
      ///     en catchant les signaux (UNIX/QT) et via unlink dans Bridge.
      ::unlink("/tmp/hole");

      // listen for incoming connection.
      if (elle::Lane::Listen(Hole::Line, connection) == elle::StatusError)
	escape("unable to listen for bridge connections");
    }

    leave();
  }

  ///
  /// this method cleans the hole.
  ///
  elle::Status		Hole::Clean()
  {
    enter();

    // XXX destroy all the instances

    leave();
  }

//
// ---------- callbacks -------------------------------------------------------
//

  ///
  /// XXX
  ///
  elle::Status		Hole::Join(const nucleus::Network&	network)
  {
    enter();

    // check if this network has already been joined. if not, join it.
    if (Hole::Networks.find(network) == Hole::Networks.end())
      {
	Holeable*					holeable;
	nucleus::Address				address;
	std::pair<Hole::Iterator, elle::Boolean>	result;

	// XXX[to change: contact the meta disciple]
	{
	  lune::Descriptor	descriptor;

	  if (descriptor.Load(network.name) == elle::StatusError)
	    escape("unable to load the descriptor");

	  address = descriptor.root;
	}
	// XXX

	// create the holeable depending on the implementation.
	holeable = new local::Local(network);

	// set the root address.
	if (holeable->Root(address) == elle::StatusError)
	  escape("unable to set the root address");

	// insert the network in the container.
	result = Hole::Networks.insert(
		   std::pair<nucleus::Network, Holeable*>(network, holeable));

	// check if the insertion was successful.
	if (result.second == false)
	  escape("unable to insert the network in the container");
      }

    // reply to the caller.
    if (Hole::Channel->Reply(elle::Inputs<TagOk>()) == elle::StatusError)
      escape("unable to reply to the caller");

    leave();
  }

  ///
  /// XXX
  ///
  elle::Status		Hole::Leave(const nucleus::Network&	network)
  {
    enter();

    // XXX use a refcount to be sure to destroy it when it is no longer used.

    // reply to the caller.
    if (Hole::Channel->Reply(elle::Inputs<TagOk>()) == elle::StatusError)
      escape("unable to reply to the caller");

    leave();
  }

  ///
  /// XXX
  ///
  elle::Status		Hole::Put(const nucleus::Network&	network,
				  const nucleus::Address&	address,
				  const
				    elle::Derivable<nucleus::Block>& derivable)
  {
    Hole::Scoutor	scoutor;
    nucleus::Block*	object;

    enter();

    // locate the network.
    if ((scoutor = Hole::Networks.find(network)) == Hole::Networks.end())
      escape("hole does not seem to be connected to this network");

    // infer the block from the derivable.
    if (derivable.Infer(object) == elle::StatusError)
      escape("unable to infer the block from the derivable");

    // forward the put request to the implementation.
    if (scoutor->second->Put(address, *object) == elle::StatusError)
      escape("unable to put the block");

    // answer to the caller.
    if (Hole::Channel->Reply(
	  elle::Inputs<TagOk>()) == elle::StatusError)
      escape("unable to answer to the caller");

    leave();
  }

  ///
  /// XXX
  ///
  elle::Status		Hole::Get(const nucleus::Network&	network,
				  const nucleus::Address&	address)
  {
    Hole::Scoutor	scoutor;
    nucleus::Block*	block;

    enter(instance(block));

    // locate the network.
    if ((scoutor = Hole::Networks.find(network)) == Hole::Networks.end())
      escape("hole does not seem to be connected to this network");

    // build the block according to the component.
    if (elle::Factory::Build(address.component, block) == elle::StatusError)
      escape("unable to build the block");

    // forward the request depending on the nature of the block which
    // the addres indicates.
    switch (address.family)
      {
	case nucleus::FamilyContentHashBlock:
	  {
	    // retrieve the immutable block.
	    if (scoutor->second->Get(address, *block) == elle::StatusError)
	      escape("unable to get the block");

	    break;
	  }
	case nucleus::FamilyPublicKeyBlock:
	case nucleus::FamilyOwnerKeyBlock:
	case nucleus::FamilyImprintBlock:
	  {
	    // retrieve the mutable block.
	    if (scoutor->second->Gather(address, *block) == elle::StatusError)
	      escape("unable to gather the block");

	    break;
	  }
      default:
	{
	  escape("unknown block family");
	}
      }

    elle::Derivable<nucleus::Block>	derivable(address.component, *block);

    // return the block to the caller.
    if (Hole::Channel->Reply(
	  elle::Inputs<TagBlock>(derivable)) == elle::StatusError)
      escape("unable to return the block to the caller");

    // waive.
    waive(block);

    // delete the block.
    delete block;

    leave();
  }

  ///
  /// XXX
  ///
  elle::Status		Hole::Erase(const nucleus::Network&	network,
				    const nucleus::Address&	address)
  {
    Hole::Scoutor	scoutor;

    enter();

    // locate the network.
    if ((scoutor = Hole::Networks.find(network)) == Hole::Networks.end())
      escape("hole does not seem to be connected to this network");

    // forward the erase request to the implementation.
    if (scoutor->second->Erase(address) == elle::StatusError)
      escape("unable to erase the block");

    // answer the caller.
    if (Hole::Channel->Reply(
	  elle::Inputs<TagOk>()) == elle::StatusError)
      escape("unable to answer to the caller");

    leave();
  }

  ///
  /// XXX
  ///
  elle::Status		Hole::Connection(elle::Door*&		door)
  {
    enter();

    // if Etoile is already considered to be connected, reject any
    // subsequent request.
    if (Hole::Channel != NULL)
      {
	// disconnect the door.
	if (door->Disconnect() == elle::StatusError)
	  escape("unable to disconnect the door");

	leave();
      }

    // otherwise, accept the connection.
    Hole::Channel = door;

    leave();
  }

  ///
  /// XXX
  ///
  elle::Status		Hole::Error(const elle::String&		error)
  {
    enter();

    // report the error.
    report(elle::Report::TypeError, error);

    // display the errors.
    show();

    // quit the program.
    elle::Program::Exit();

    leave();
  }

}
