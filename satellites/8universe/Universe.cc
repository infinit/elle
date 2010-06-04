//
// ---------- header ----------------------------------------------------------
//
// project       8universe
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8universe/Universe.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [mon may 10 09:41:44 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8universe/Universe.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "8universe";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a new universe by using the user 'name' as the
  /// initial user.
  ///
  elle::Status		Universe::Create(const elle::String&	name,
					 const elle::Address&	network,
					 const elle::String&	owner)
  {
    lune::Authority		authority;
    lune::Identity		identity;
    etoile::kernel::Object	directory;

    enter();

    //
    // test the arguments.
    //
    {
      // test the name argument.
      if (name.empty() == true)
	escape("unable to create a universe without a name");

      // test the network argument.
      if (network == elle::Address::Null)
	escape("unable to create a universe without a bootstrap node");

      // test the owner argument.
      if (owner.empty() == true)
	escape("unable to create a universe without a owner");
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
      etoile::user::Agent	agent;

      // create the agent based on the owner's key pair.
      //
      // this agent will be used to sign the root directory's object.
      if (agent.Create(identity.pair) == elle::StatusError)
	escape("unable to create the agent");

      // create directory object, setting the user's as the owner.
      if (directory.Create(etoile::GenreDirectory,
			   agent.K) == elle::StatusError)
	escape("unable to create the object directory");

      // seal the directory.
      if (directory.Seal(agent) == elle::StatusError)
	escape("unable to seal the object");

      // compute the directory's address.
      if (directory.Bind() == elle::StatusError)
	escape("unable to bind the object to an address");
    }

    //
    // create the universe's layout.
    //
    {
      // create the universe directory.
      if (::mkdir((lune::Lune::Universes +
		   elle::System::Path::Separator +
		   name).c_str(), 0700) == -1)
	escape(::strerror(errno));

      // create the universe's reserve directory.
      if (::mkdir((lune::Lune::Universes +
		   elle::System::Path::Separator +
		   name +
		   elle::System::Path::Separator +
		   lune::Lune::Reserve).c_str(), 0700) == -1)
	escape(::strerror(errno));

      // create the universe's hole directory.
      if (::mkdir((lune::Lune::Universes +
		   elle::System::Path::Separator +
		   name +
		   elle::System::Path::Separator +
		   lune::Lune::Hole).c_str(), 0700) == -1)
	escape(::strerror(errno));
    }

    //
    // create the universe's memento.
    //
    {
      lune::Memento	memento;

      // create the memento.
      if (memento.Create(name,
			 directory.address,
			 network) == elle::StatusError)
	escape("unable to create the universe's memento");

      // seal the memento.
      if (memento.Seal(authority) == elle::StatusError)
	escape("unable to seal the memento");

      // store the memento.
      if (memento.Store(name) == elle::StatusError)
	escape("unable to store the memento file");
    }

    //
    // store the root directory block now that the universe exists.
    //
    {
      // store the object.
      if (etoile::hole::Hole::Put(name,
				  directory.address,
				  &directory) == elle::StatusError)
	escape("unable to store the directory block");
    }

    leave();
  }

  ///
  /// this method destroys an existing universe.
  ///
  elle::Status		Universe::Destroy(const elle::String&	name)
  {
    enter();

    // XXX

    leave();
  }

  ///
  /// this method retrieves and displays information on the given universe.
  ///
  elle::Status		Universe::Information(const elle::String& name)
  {
    lune::Memento	memento;
    lune::Authority	authority;

    enter();

    //
    // test the arguments.
    //
    {
      // test the name.
      if (name.empty() == true)
	escape("please specify the name of the universe");
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
    // retrieve the memento.
    //
    {
      // load the memento.
      if (memento.Load(name) == elle::StatusError)
	escape("unable to load the memento");

      // validate the memento.
      if (memento.Validate(authority) != elle::StatusTrue)
	escape("unable to validate the memento");
    }

    //
    // display information.
    //
    {
      // dump the memento.
      if (memento.Dump() == elle::StatusError)
	escape("unable to dump the memento");
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
    Universe::Operation		operation;
    elle::Character		option;
    elle::String		name;
    elle::String		owner;
    elle::Address		network;

    enter();

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::StatusError)
      escape("unable to initialize Lune");

    // initialize the Etoile.
    if (etoile::Etoile::Initialize() == elle::StatusError)
      escape("unable to initialize Etoile");

    // initialize the operation.
    operation = Universe::OperationUnknown;

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
			 "create a new universe",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('d',
			 "destroy",
			 "destroy an existing universe",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('x',
			 "information",
			 "print information on the universe",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('n',
			 "name",
			 "specifies the universe name",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('o',
			 "owner",
			 "specifies the name of the universe's owner",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('b',
			 "bootstrap",
			 "specifies the universe's bootstrap node's network "
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
	      if (operation != Universe::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the create operation cannot be set concurrently "
			 "to another operation");
		}

	      operation = Universe::OperationCreate;

	      break;
	    }
	  case 'd':
	    {
	      // check if the operation has already been set up.
	      if (operation != Universe::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the destroy operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Universe::OperationDestroy;

	      break;
	    }
	  case 'x':
	    {
	      // check if the operation has already been set up.
	      if (operation != Universe::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the print operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Universe::OperationInformation;

	      break;
	    }
	  case 'n':
	    {
	      // assign the name.
	      name.assign(optarg);

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
	      if (network.Create(optarg) == elle::StatusError)
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

    // trigger the operation.
    switch (operation)
      {
      case Universe::OperationCreate:
	{
	  // create the universe.
	  if (Universe::Create(name, network, owner) == elle::StatusError)
	    escape("unable to create the universe");

	  // display a message.
	  std::cout << "The universe has been created successfully!"
		    << std::endl;

	  break;
	}
      case Universe::OperationDestroy:
	{
	  // destroy the universe.
	  if (Universe::Destroy(name) == elle::StatusError)
	    escape("unable to destroy the universe");

	  // display a message.
	  std::cout << "The universe has been destroyed successfully!"
		    << std::endl;

	  break;
	}
      case Universe::OperationInformation:
	{
	  // get information on the universe.
	  if (Universe::Information(name) == elle::StatusError)
	    escape("unable to retrieve information on the universe");

	  break;
	}
      case Universe::OperationUnknown:
      default:
	{
	  // display the usage.
	  parser->Usage();

	  escape("please specify an operation to perform");
	}
      }

    // delete the parser.
    delete parser;

    // clean the Etoile.
    if (etoile::Etoile::Clean() == elle::StatusError)
      escape("unable to clean Etoile");

    // clean Lune
    if (lune::Lune::Clean() == elle::StatusError)
      escape("unable to clean Lune");

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
