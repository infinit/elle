//
// ---------- header ----------------------------------------------------------
//
// project       8network
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8network/Network.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [wed may 25 10:07:10 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8network/Network.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "8network";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a new network by using the user 'name' as the
  /// initial user.
  ///
  elle::Status		Network::Create(const elle::String&	name,
					const hole::Model&	model,
					const elle::Address&	boot,
					const elle::String&	owner)
  {
    lune::Authority	authority;
    lune::Descriptor	descriptor;
    lune::Identity	identity;
    nucleus::Object	directory;
    nucleus::Address	address;
    nucleus::Network	network;

    enter();

    //
    // test the arguments.
    //
    {
      // test the name argument.
      if (name.empty() == true)
	escape("unable to create a network without a name");

      // does the network already exist.
      if (descriptor.Exist(name) == elle::StatusTrue)
	escape("this network seems to already exist");

      // check the model.
      if (model == hole::ModelUnknown)
	escape("please specify the model of the network");

      // test the network argument.
      if (boot == elle::Address::Null)
	escape("unable to create a network without a bootstrap node");

      // test the owner argument.
      if (owner.empty() == true)
	escape("unable to create a network without a owner");

      // does the owner user exist.
      if (identity.Exist(owner) == elle::StatusFalse)
	escape("the owner user does not seem to exist");

      // check if the authority exists.
      if (authority.Exist() == elle::StatusFalse)
	escape("unable to locate the authority file");
    }

    //
    // retrieve the authority.
    //
    {
      elle::String		prompt;
      elle::String		pass;

      // prompt the user for the passphrase.
      prompt = "Enter passphrase for the authority: ";
      pass = elle::String(::getpass(prompt.c_str()));

      // load the authority.
      if (authority.Load() == elle::StatusError)
	escape("unable to load the authority");

      // decrypt the authority.
      if (authority.Decrypt(pass) == elle::StatusError)
	escape("unable to decrypt the authority");
    }

    //
    // retrieve the owner's identity.
    //
    {
      elle::String		prompt;
      elle::String		pass;

      // prompt the user for the passphrase.
      prompt = "Enter passphrase for keypair '" + owner + "': ";
      pass = elle::String(::getpass(prompt.c_str()));

      // load the identity.
      if (identity.Load(owner) == elle::StatusError)
	escape("unable to load the owner's identity");

      // decrypt the authority.
      if (identity.Decrypt(pass) == elle::StatusError)
	escape("unable to decrypt the identity");
    }

    //
    // create the root directory.
    //
    {
      // create the network object.
      if (network.Create(name) == elle::StatusError)
	escape("unable to create the network object");

      // place the block in the given network.
      if (directory.Place(network) == elle::StatusError)
	escape("unable to place the directory");

      // create directory object, setting the user's as the owner.
      if (directory.Create(nucleus::GenreDirectory,
			   identity.pair.K) == elle::StatusError)
	escape("unable to create the object directory");

      // seal the directory.
      if (directory.Seal(identity.pair.k) == elle::StatusError)
	escape("unable to seal the object");

      // compute the directory's address.
      if (directory.Bind(address) == elle::StatusError)
	escape("unable to bind the object to an address");
    }

    //
    // create the network's memento.
    //
    {
      // create the descriptor.
      if (descriptor.Create(name,
			    model,
			    address,
			    boot) == elle::StatusError)
	escape("unable to create the network's descriptor");

      // seal the descriptor.
      if (descriptor.Seal(authority) == elle::StatusError)
	escape("unable to seal the descriptor");

      // store the descriptor.
      if (descriptor.Store(name) == elle::StatusError)
	escape("unable to store the descriptor file");
    }

    //
    // store the root directory block now that the network exists.
    //
    {
      // store the block.
      if (directory.Store(network,
			  address) == elle::StatusError)
	escape("unable to store the block");
    }

    leave();
  }

  ///
  /// this method destroys an existing network.
  ///
  elle::Status		Network::Destroy(const elle::String&	name)
  {
    enter();

    //
    // remove the descriptor.
    //
    {
      lune::Descriptor	descriptor;
      elle::Path	path;

      // does the network exist.
      if (descriptor.Exist(name) == elle::StatusTrue)
	{
	  // remove the descriptor.
	  if (descriptor.Erase(name) == elle::StatusError)
	    escape("unable to erase the descriptor");
	}
    }

    //
    // destroy the reserve, if necessary
    //
    {
      elle::Path	path;

      // create the reserve path.
      if (path.Create(lune::Lune::Network::Reserve::Root) == elle::StatusError)
	escape("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::Piece("%NETWORK%", name)) == elle::StatusError)
	escape("unable to complete the path");

      // if the reserve exists, clear it and remove it.
      if (elle::Directory::Exist(path) == elle::StatusTrue)
	{
	  // clear the reserve content.
	  if (elle::Directory::Clear(path) == elle::StatusError)
	    escape("unable to clear the directory");

	  // remove the directory.
	  if (elle::Directory::Remove(path) == elle::StatusError)
	    escape("unable to remove the directory");
	}
    }

    //
    // destroy the shelter.
    //
    {
      elle::Path	path;

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::Root) == elle::StatusError)
	escape("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::Piece("%NETWORK%", name)) == elle::StatusError)
	escape("unable to complete the path");

      // if the shelter exists, clear it and remove it.
      if (elle::Directory::Exist(path) == elle::StatusTrue)
	{
	  // clear the reserve content.
	  if (elle::Directory::Clear(path) == elle::StatusError)
	    escape("unable to clear the directory");

	  // remove the directory.
	  if (elle::Directory::Remove(path) == elle::StatusError)
	    escape("unable to remove the directory");
	}
    }

    //
    // remove the network directory.
    //
    {
      elle::Path	path;

      // create the network path.
      if (path.Create(lune::Lune::Network::Root) == elle::StatusError)
	escape("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::Piece("%NETWORK%", name)) == elle::StatusError)
	escape("unable to complete the path");

      // clear the network directory content.
      if (elle::Directory::Clear(path) == elle::StatusError)
	escape("unable to clear the directory");

      // remove the directory.
      if (elle::Directory::Remove(path) == elle::StatusError)
	escape("unable to remove the directory");
    }

    leave();
  }

  ///
  /// this method retrieves and displays information on the given network.
  ///
  elle::Status		Network::Information(const elle::String& name)
  {
    lune::Descriptor	descriptor;
    lune::Authority	authority;

    enter();

    //
    // test the arguments.
    //
    {
      // test the name.
      if (name.empty() == true)
	escape("please specify the name of the network");

      // does the network exist.
      if (descriptor.Exist(name) == elle::StatusFalse)
	escape("this network does not seem to exist");
    }

    //
    // retrieve the authority.
    //
    {
      elle::PublicKey	K;

      // restore the authority's public key.
      if (K.Restore(Infinit::Authority) == elle::StatusError)
	escape("unable to restore the authority's public key");

      // create the authority based on the hard-coded public key.
      if (authority.Create(K) == elle::StatusError)
	escape("unable to create the authority");
    }

    //
    // retrieve the descriptor.
    //
    {
      // load the descripto.
      if (descriptor.Load(name) == elle::StatusError)
	escape("unable to load the descriptor");

      // validate the descriptor.
      if (descriptor.Validate(authority) != elle::StatusTrue)
	escape("unable to validate the descriptor");
    }

    //
    // display information.
    //
    {
      // dump the descriptor.
      if (descriptor.Dump() == elle::StatusError)
	escape("unable to dump the descriptor");
    }

    leave();
  }

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  elle::Status		Main(elle::Natural32			argc,
			     elle::Character*			argv[])
  {
    elle::Parser*		parser;
    Network::Operation		operation;
    elle::Character		option;
    elle::String		name;
    hole::Model			model;
    elle::String		owner;
    elle::Address		boot;

    enter();

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // initialize the operation.
    operation = Network::OperationUnknown;

    // initialize the model.
    model = hole::ModelUnknown;

    // allocate a new parser.
    parser = new elle::Parser(argc, argv);

    // set up the parser.
    if (parser->Register('h',
			 "help",
			 "display the help",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('c',
			 "create",
			 "create a new network",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('d',
			 "destroy",
			 "destroy an existing network",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('x',
			 "information",
			 "print information on the network",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('n',
			 "name",
			 "specifies the network name",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('m',
			 "model",
			 "specifies the network model: local, remote, kool "
			 "etc.",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('o',
			 "owner",
			 "specifies the name of the network's owner",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('b',
			 "bootstrap",
			 "specifies the bootstrap node's network "
			 "address of the form 'host:port'",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    // parse.
    while (parser->Parse(option) == elle::StatusTrue)
      {
	switch (option)
	  {
	  case 'h':
	    {
	      // display the usage.
	      parser->Usage();

	      // quit.
	      leave();
	    }
	  case 'c':
	    {
	      // check if the operation has already been set up.
	      if (operation != Network::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the create operation cannot be set concurrently "
			 "to another operation");
		}

	      operation = Network::OperationCreate;

	      break;
	    }
	  case 'd':
	    {
	      // check if the operation has already been set up.
	      if (operation != Network::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the destroy operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Network::OperationDestroy;

	      break;
	    }
	  case 'x':
	    {
	      // check if the operation has already been set up.
	      if (operation != Network::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the print operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Network::OperationInformation;

	      break;
	    }
	  case 'n':
	    {
	      // assign the name.
	      name.assign(optarg);

	      break;
	    }
	  case 'm':
	    {
	      elle::String	m(optarg);

	      // assign the model.
	      if (m == "local")
		model = hole::ModelLocal;
	      else if (m == "remote")
		model = hole::ModelRemote;
	      else if (m == "kool")
		model = hole::ModelKool;
	      else
		escape("unknown model");

	      break;
	    }
	  case 'o':
	    {
	      // assign the owner.
	      owner.assign(optarg);

	      break;
	    }
	  case 'b':
	    {
	      // create the node.
	      if (boot.Create(optarg) == elle::StatusError)
		escape("unable to create the bootstrap address");

	      break;
	    }
	  case '?':
	    {
	      // display the usage.
	      parser->Usage();

	      escape("unknown option");
	    }
	  case ':':
	    {
	      // display the usage.
	      parser->Usage();

	      escape("missing argument");
	    }
	  default:
	    {
	      escape("an error occured while parsing the options");
	    }
	  }
      }

    // initialize the nucleus library.
    if (nucleus::Nucleus::Initialize() == elle::StatusError)
      escape("unable to initialize Nucleus");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::StatusError)
      escape("unable to initialize Lune");

    // trigger the operation.
    switch (operation)
      {
      case Network::OperationCreate:
	{
	  // create the network.
	  if (Network::Create(name, model, boot, owner) == elle::StatusError)
	    escape("unable to create the network");

	  // display a message.
	  std::cout << "The network has been created successfully!"
		    << std::endl;

	  break;
	}
      case Network::OperationDestroy:
	{
	  // destroy the network.
	  if (Network::Destroy(name) == elle::StatusError)
	    escape("unable to destroy the network");

	  // display a message.
	  std::cout << "The network has been destroyed successfully!"
		    << std::endl;

	  break;
	}
      case Network::OperationInformation:
	{
	  // get information on the network.
	  if (Network::Information(name) == elle::StatusError)
	    escape("unable to retrieve information on the network");

	  break;
	}
      case Network::OperationUnknown:
      default:
	{
	  // display the usage.
	  parser->Usage();

	  escape("please specify an operation to perform");
	}
      }

    // delete the parser.
    delete parser;

    // clean Lune
    if (lune::Lune::Clean() == elle::StatusError)
      escape("unable to clean Lune");

    // clean the nucleus library.
    if (nucleus::Nucleus::Clean() == elle::StatusError)
      escape("unable to clean Nucleus");

    // clean Elle.
    if (elle::Elle::Clean() == elle::StatusError)
      escape("unable to clean Elle");

    leave();
  }

}

//
// ---------- main ------------------------------------------------------------
//

///
/// this is the program entry point.
///
int			main(int				argc,
                             char**				argv)
{
  application::Main(argc, argv);

  expose();

  return (0);
}
