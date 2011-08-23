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
// updated       julien quintard   [fri aug 12 13:56:59 2011]
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
					const elle::String&	administrator)
  {
    lune::Authority	authority;
    lune::Descriptor	descriptor;
    lune::Identity	identity;
    nucleus::Object	directory;
    nucleus::Network	network;
    nucleus::Address	address;

    enter();

    //
    // test the arguments.
    //
    {
      // does the network already exist.
      if (descriptor.Exist(name) == elle::StatusTrue)
	escape("this network seems to already exist");

      // check the model.
      if (model == hole::Model::Null)
	escape("please specify the model of the network");

      // does the administrator user exist.
      if (identity.Exist(administrator) == elle::StatusFalse)
	escape("the administrator user does not seem to exist");

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

      if (elle::Console::Input(
            pass,
	    prompt,
	    elle::Console::OptionPassword) == elle::StatusError)
	escape("unable to read the input");

      // load the authority.
      if (authority.Load() == elle::StatusError)
	escape("unable to load the authority");

      // decrypt the authority.
      if (authority.Decrypt(pass) == elle::StatusError)
	escape("unable to decrypt the authority");
    }

    //
    // retrieve the administrator's identity.
    //
    {
      elle::String		prompt;
      elle::String		pass;

      // prompt the user for the passphrase.
      prompt = "Enter passphrase for keypair '" + administrator + "': ";

      if (elle::Console::Input(
            pass,
	    prompt,
	    elle::Console::OptionPassword) == elle::StatusError)
	escape("unable to read the input");

      // load the identity.
      if (identity.Load(administrator) == elle::StatusError)
	escape("unable to load the administrator's identity");

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

      // create directory object, setting the user's as the administrator.
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
    // create the network's descriptor.
    //
    {
      // create the descriptor.
      if (descriptor.Create(name,
			    model,
			    address) == elle::StatusError)
	escape("unable to create the network's descriptor");

      // seal the descriptor.
      if (descriptor.Seal(authority) == elle::StatusError)
	escape("unable to seal the descriptor");

      // push the attributes.
      if (descriptor.Push() == elle::StatusError)
	escape("unable to push the descriptor's attributes");

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

    enter();

    //
    // test the arguments.
    //
    {
      // does the network exist.
      if (descriptor.Exist(name) == elle::StatusFalse)
	escape("this network does not seem to exist");
    }

    //
    // retrieve the descriptor.
    //
    {
      // load the descriptor.
      if (descriptor.Load(name) == elle::StatusError)
	escape("unable to load the descriptor");

      // pull the attributes.
      if (descriptor.Pull() == elle::StatusError)
	escape("unable to pull the descriptor's attributes");

      // validate the descriptor.
      if (descriptor.Validate(Infinit::Authority) == elle::StatusError)
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
    Network::Operation	operation;

    enter(instance(Infinit::Parser));

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // initialize the nucleus library.
    if (nucleus::Nucleus::Initialize() == elle::StatusError)
      escape("unable to initialize Nucleus");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::StatusError)
      escape("unable to initialize Lune");

    // initialize Infinit.
    if (Infinit::Initialize() == elle::StatusError)
      escape("unable to initialize Infinit");

    // initialize the Etoile library.
    if (etoile::Etoile::Initialize() == elle::StatusError)
      escape("unable to initialize Etoile");

    // initialize the operation.
    operation = Network::OperationUnknown;

    // allocate a new parser.
    Infinit::Parser = new elle::Parser(argc, argv);

    // specify a program description.
    if (Infinit::Parser->Description(Infinit::Copyright) == elle::StatusError)
      escape("unable to set the description");

    // register the options.
    if (Infinit::Parser->Register(
          "Help",
	  'h',
	  "help",
	  "display the help",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Create",
	  'c',
	  "create",
	  "create a new network",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Destroy",
	  'd',
	  "destroy",
	  "destroy an existing network",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Information",
	  'x',
	  "information",
	  "display information regarding a network",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Name",
	  'n',
	  "name",
	  "specify the network name",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Model",
	  'm',
	  "model",
	  "specify the network model: Local, Remote, Kool etc.",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Administrator",
	  'a',
	  "administrator",
	  "specify the network administrator",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // parse.
    if (Infinit::Parser->Parse() == elle::StatusError)
      escape("unable to parse the command line");

    // test the option.
    if (Infinit::Parser->Test("Help") == elle::StatusTrue)
      {
	// display the usage.
	Infinit::Parser->Usage();

	// quit.
	leave();
      }

    // check the mutually exclusive options.
    if ((Infinit::Parser->Test("Create") == elle::StatusTrue) &&
	(Infinit::Parser->Test("Destroy") == elle::StatusTrue) &&
	(Infinit::Parser->Test("Information") == elle::StatusTrue))
      {
	// display the usage.
	Infinit::Parser->Usage();

	escape("the create, destroy and information options are "
	       "mutually exclusive");
      }

    // test the option.
    if (Infinit::Parser->Test("Create") == elle::StatusTrue)
      operation = Network::OperationCreate;

    // test the option.
    if (Infinit::Parser->Test("Destroy") == elle::StatusTrue)
      operation = Network::OperationDestroy;

    // test the option.
    if (Infinit::Parser->Test("Information") == elle::StatusTrue)
      operation = Network::OperationInformation;

    // trigger the operation.
    switch (operation)
      {
      case Network::OperationCreate:
	{
	  elle::String		name;
	  elle::String		string;
	  hole::Model		model;
	  elle::String		administrator;

	  // retrieve the name.
	  if (Infinit::Parser->Value("Name", name) == elle::StatusError)
	    escape("unable to retrieve the name value");

	  // retrieve the model.
	  if (Infinit::Parser->Value("Model", string) == elle::StatusError)
	    escape("unable to retrieve the model value");

	  // build the model.
	  if (model.Create(string) == elle::StatusError)
	    escape("unable to create the model");

	  // retrieve the administrator.
	  if (Infinit::Parser->Value("Administrator",
				     administrator) == elle::StatusError)
	    escape("unable to retrieve the administrator value");

	  // create the network.
	  if (Network::Create(name, model, administrator) == elle::StatusError)
	    escape("unable to create the network");

	  // display a message.
	  std::cout << "The network has been created successfully!"
		    << std::endl;

	  break;
	}
      case Network::OperationDestroy:
	{
	  elle::String		name;

	  // retrieve the name.
	  if (Infinit::Parser->Value("Name", name) == elle::StatusError)
	    escape("unable to retrieve the name value");

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
	  elle::String		name;

	  // retrieve the name.
	  if (Infinit::Parser->Value("Name", name) == elle::StatusError)
	    escape("unable to retrieve the name value");

	  // get information on the network.
	  if (Network::Information(name) == elle::StatusError)
	    escape("unable to retrieve information on the network");

	  break;
	}
      case Network::OperationUnknown:
      default:
	{
	  // display the usage.
	  Infinit::Parser->Usage();

	  escape("please specify an operation to perform");
	}
      }

    // delete the parser.
    delete Infinit::Parser;

    // waive.
    waive(Infinit::Parser);

    // clean the Etoile.
    if (etoile::Etoile::Clean() == elle::StatusError)
      escape("unable to clean Etoile");

    // clean Infinit.
    if (Infinit::Clean() == elle::StatusError)
      escape("unable to clean Infinit");

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
  try
    {
      if (application::Main(argc, argv) == elle::StatusError)
	{
	  show();

	  return (1);
	}
    }
  catch (std::exception& e)
    {
      std::cout << "The program has been terminated following "
		<< "a fatal error (" << e.what() << ")." << std::endl;

      return (1);
    }

  return (0);
}
