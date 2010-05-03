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
// updated       julien quintard   [sat may  1 20:55:03 2010]
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
  const Character	Component[] = "8universe";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a new universe by using the user 'name' as the
  /// initial user.
  ///
  Status		Universe::Create(const String&		name,
					 const String&		user)
  {
    KeyPair			pair;
    etoile::kernel::Object	directory;

    enter();

    // test the arguments.
    if ((name.empty() == true) || (user.empty() == true))
      escape("unable to create a universe without a name and/or a user");

    //
    // retrieve the initial user's key pair.
    //
    {
      String		path =
	Lune::Keys + System::Path::Separator + user + ".pair";
      String		prompt;
      String		pass;

      // prompt the user for the passphrase.
      prompt = "Enter passphrase for keypair '" + path + "': ";
      pass = String(::getpass(prompt.c_str()));

      // load the key pair.
      if (pair.Load(path, pass) == StatusError)
	escape("unable to load the key pair");
    }

    //
    // create a directory.
    //
    {
      etoile::user::Agent	agent;

      // create the agent.
      if (agent.Create(pair) == StatusError)
	escape("unable to create the agent");

      // create directory object.
      if (directory.Create(GenreDirectory, agent.K) == StatusError)
	escape("unable to create the object directory");

      // seal the data and meta data.
      if (directory.Seal(agent) == StatusError)
	escape("unable to seal the object");

      // compute the address.
      if (directory.Bind() == StatusError)
	escape("unable to bind the object to an address");
    }

    //
    // store the directory object.
    //
    {
      // store the block.
      if (etoile::hole::Hole::Put(directory.address,
				  &directory) == StatusError)
	escape("unable to store the directory block");
    }

    //
    // hack for bootstrapping purposes: store the root directory
    // address in a .root file.
    //
    {
      String		path =
	Lune::Universes + System::Path::Separator + name + ".uni";

      // store the universe's address.
      if (directory.address.Store(path) == StatusError)
	escape("unable to store the universe file");
    }

    leave();
  }

  ///
  /// this method destroys an existing universe.
  ///
  Status		Universe::Destroy(const String&		name)
  {
    enter();

    // XXX

    leave();
  }

  ///
  /// this method retrieves and displays information on the given universe.
  ///
  Status		Universe::Information(const String&	name)
  {
    enter();

    // XXX

    leave();
  }

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  Status		Main(Natural32				argc,
			     Character*				argv[])
  {
    Parser*		parser;
    Universe::Operation	operation;
    Character		option;
    String		name;
    String		user;

    enter();

    // initialize the Elle library.
    if (Elle::Initialize() == StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (Program::Setup() == StatusError)
      escape("unable to set up the program");

    // initialize the Lune library.
    if (Lune::Initialize() == StatusError)
      escape("unable to initialize Lune");

    // initialize the Etoile.
    if (Etoile::Initialize() == StatusError)
      escape("unable to initialize Etoile");

    // initialize the operation.
    operation = Universe::OperationUnknown;

    // allocate a new parser.
    parser = new Parser(argc, argv);

    // set up the parser.
    if (parser->Register('h',
			 "help",
			 "display the help",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('c',
			 "create",
			 "create a new universe",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('d',
			 "destroy",
			 "destroy an existing universe",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('i',
			 "information",
			 "print information on the universe",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('n',
			 "name",
			 "specifies the universe name",
			 Parser::TypeRequired) == StatusError)
      escape("unable to register the option");

    if (parser->Register('u',
			 "user",
			 "specifies the name of the universe's creator",
			 Parser::TypeRequired) == StatusError)
      escape("unable to register the option");

    // parse.
    while (parser->Parse(option) == StatusTrue)
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
	  case 'i':
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
	  case 'u':
	    {
	      // assign the user.
	      user.assign(optarg);

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
	  if (Universe::Create(name, user) == StatusError)
	    escape("unable to create the universe");

	  break;
	}
      case Universe::OperationDestroy:
	{
	  // destroy the universe.
	  if (Universe::Destroy(name) == StatusError)
	    escape("unable to destroy the universe");

	  break;
	}
      case Universe::OperationInformation:
	{
	  // get information on the universe.
	  if (Universe::Information(name) == StatusError)
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
    if (Etoile::Clean() == StatusError)
      escape("unable to clean Etoile");

    // clean Lune
    if (Lune::Clean() == StatusError)
      escape("unable to clean Lune");

    // clean Elle.
    if (Elle::Clean() == StatusError)
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
