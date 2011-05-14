//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Hole.cc
//
// created       julien quintard   [thu may 12 14:43:49 2011]
// updated       julien quintard   [sat may 14 13:10:03 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Hole.hh>

#include <hole/Manifest.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this string represents the door's name to Etoile.
    ///
    const elle::String		Hole::Line("hole");

    ///
    /// the door to Hole.
    ///
    elle::Door*			Hole::Channel = NULL;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the hole module.
    ///
    elle::Status	Hole::Initialize()
    {
      elle::Callback<const elle::String>	error(&Hole::Error);

      enter();

      // allocate a door.
      Hole::Channel = new elle::Door;

      // create the door.
      if (Hole::Channel->Create() == elle::StatusError)
	escape("unable to create the door");

      // monitor the socket.
      if (Hole::Channel->Monitor(error) == elle::StatusError)
	escape("unable to create the door");

      // connect the door.
      if (Hole::Channel->Connect(Hole::Line) == elle::StatusError)
	escape("unable to connect to Hole");

      leave();
    }

    ///
    /// this method cleans the hole module.
    ///
    elle::Status	Hole::Clean()
    {
      enter();

      // delete the channel, if present.
      if (Hole::Channel != NULL)
	delete Hole::Channel;

      leave();
    }

    ///
    /// this callback is triggered whenever an error on Hole.
    ///
    elle::Status	Hole::Error(const elle::Report&		report)
    {
      enter();

      // report the error.
      report(elle::Report::TypeError, report);

      // display the errors.
      show();

      // quit the program.
      elle::Program::Exit();

      leave();
    }

    ///
    /// this callback is triggered whenever an error occurs on the socket.
    ///
    elle::Status	Hole::Error(const elle::String&		error)
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

    ///
    /// this method instructs the Hole to join the given network though
    /// Hole may already be connected to this network through another
    /// application.
    ///
    elle::Status	Hole::Join(const nucleus::Network&	network)
    {
      enter();

      // request hole to join the network.
      if (Hole::Channel->Call(
	    elle::Inputs<hole::TagJoin>(network),
	    elle::Outputs<hole::TagOk>()) == elle::StatusError)
	escape("unable to join the network");

      leave();
    }

    ///
    /// this method instructs the Hole to leave a network.
    ///
    elle::Status	Hole::Leave(const nucleus::Network&	network)
    {
      enter();

      // request hole to leave the network.
      if (Hole::Channel->Call(
	    elle::Inputs<hole::TagLeave>(network),
	    elle::Outputs<hole::TagOk>()) == elle::StatusError)
	escape("unable to join the network");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Hole::Put(const nucleus::Network&	network,
				  const nucleus::Address&	address,
				  const nucleus::Block&		block)
    {
      elle::Derivable<nucleus::Block>	derivable(address.component, block);

      enter();

      // request hole to put the block.
      if (Hole::Channel->Call(
	    elle::Inputs<hole::TagPut>(network, address, derivable),
	    elle::Outputs<hole::TagOk>()) == elle::StatusError)
	escape("unable to put the block in the network");

      /* XXX
      if (block.Store(network,
		      address) == elle::StatusError)
	escape("unable to store the block");
      */

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Hole::Get(const nucleus::Network&	network,
				  const nucleus::Address&	address,
				  nucleus::Block&		block)
    {
      elle::Derivable<nucleus::Block>	derivable(block);
      nucleus::Block*			object;

      enter();

      // request hole to get the block associated with the given
      // network and address.
      if (Hole::Channel->Call(
	    elle::Inputs<hole::TagGet>(network, address),
	    elle::Outputs<hole::TagBlock>(derivable)) == elle::StatusError)
	escape("unable to get the block from the network");

      // retrieve the derivable's object.
      if (derivable.Infer(object) == elle::StatusError)
	escape("unable to infer the block from the derivable");

      /* XXX
      if (block.Exist(network,
		      address) == elle::StatusFalse)
	escape("the block does not seem to exist");
      if (block.Load(network,
		     address) == elle::StatusError)
	escape("unable to load the block");
      if (block.Validate(address) == elle::StatusFalse)
	escape("the block seems to be invalid");
      */

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Hole::Erase(const nucleus::Network&	network,
				    const nucleus::Address&	address)
    {
      enter();

      // request hole to erase the block from the network.
      if (Hole::Channel->Call(
	    elle::Inputs<hole::TagErase>(network, address),
	    elle::Outputs<hole::TagOk>()) == elle::StatusError)
	escape("unable to erase the block from the network");

      /* XXX
      nucleus::Block	block;
      if (block.Erase(network, address) == elle::StatusError)
	escape("unable to erase the block");
      */

      leave();
    }

  }
}
