//
// ---------- header ----------------------------------------------------------
//
// project       8authority
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8authority/Authority.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [sun jun 19 17:46:33 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8authority/Authority.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "8authority";

  ///
  /// this value defines the authority key pair length.
  ///
  /// the length is kept high in order to make attacks more difficult.
  ///
  const elle::Natural32		Authority::Length = 4096;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a new authority by using the user 'name' as the
  /// initial user.
  ///
  elle::Status		Authority::Create()
  {
    elle::String	prompt;
    elle::String	pass;
    elle::KeyPair	pair;
    lune::Authority	authority;

    enter();

    // prompt the user for the passphrase.
    prompt = "Enter passphrase for the authority keypair: ";

    if (elle::Console::Input(
          pass,
	  prompt,
	  elle::Console::OptionPassword) == elle::StatusError)
      escape("unable to read the input");

    // generate the authority key pair.
    if (pair.Generate(Authority::Length) == elle::StatusError)
      escape("unable to generate the key pair");

    // create the authority with the generated key pair.
    if (authority.Create(pair) == elle::StatusError)
      escape("unable to create the authority");

    // encrypt the authority.
    if (authority.Encrypt(pass) == elle::StatusError)
      escape("unable to encrypt the authority");

    // store the authority.
    if (authority.Store() == elle::StatusError)
      escape("unable to store the authority");

    leave();
  }

  ///
  /// this method destroys the existing authority.
  ///
  elle::Status		Authority::Destroy()
  {
    elle::String	prompt;
    elle::KeyPair	pair;
    lune::Authority	authority;

    enter();

    // erase the authority file.
    if (authority.Erase() == elle::StatusError)
      escape("unable to erase the authority");

    leave();
  }

  ///
  /// this method retrieves and displays information on the authority.
  ///
  elle::Status		Authority::Information()
  {
    elle::String	prompt;
    elle::String	pass;
    lune::Authority	authority;
    elle::Unique	unique;

    enter();

    // check if the authority exists.
    if (authority.Exist() == elle::StatusFalse)
      escape("unable to locate the authority file");

    // prompt the user for the passphrase.
    prompt = "Enter passphrase for the authority keypair: ";

    if (elle::Console::Input(
          pass,
	  prompt,
	  elle::Console::OptionPassword) == elle::StatusError)
      escape("unable to read the input");

    // load the authority.
    if (authority.Load() == elle::StatusError)
      escape("unable to load the authority file");

    // decrypt the authority.
    if (authority.Decrypt(pass) == elle::StatusError)
      escape("unable to decrypt the authority");

    // dump the authority.
    if (authority.Dump() == elle::StatusError)
      escape("unable to dump the authority");

    // retrive the public key's unique.
    if (authority.K.Save(unique) == elle::StatusError)
      escape("unable to save the authority's public key");

    // dump the public key's unique so that it can be easily hard-coded in the
    // infinit software sources.
    std::cout << "[Unique] " << unique << std::endl;

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
    Authority::Operation	operation;
    elle::Character		option;

    enter();

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // initialize the operation.
    operation = Authority::OperationUnknown;

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
			 "create the authority",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('d',
			 "destroy",
			 "destroy the existing authority",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('x',
			 "information",
			 "print information on the authority",
			 elle::Parser::TypeNone) == elle::StatusError)
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
	      if (operation != Authority::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the create operation cannot be set concurrently "
			 "to another operation");
		}

	      operation = Authority::OperationCreate;

	      break;
	    }
	  case 'd':
	    {
	      // check if the operation has already been set up.
	      if (operation != Authority::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the destroy operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Authority::OperationDestroy;

	      break;
	    }
	  case 'x':
	    {
	      // check if the operation has already been set up.
	      if (operation != Authority::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the print operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Authority::OperationInformation;

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

    // initialize the Etoile.
    if (etoile::Etoile::Initialize() == elle::StatusError)
      escape("unable to initialize Etoile");

    // trigger the operation.
    switch (operation)
      {
      case Authority::OperationCreate:
	{
	  // create the authority.
	  if (Authority::Create() == elle::StatusError)
	    escape("unable to create the authority");

	  // display a message.
	  std::cout << "The authority has been created successfully!"
		    << std::endl;

	  break;
	}
      case Authority::OperationDestroy:
	{
	  // destroy the authority.
	  if (Authority::Destroy() == elle::StatusError)
	    escape("unable to destroy the authority");

	  // display a message.
	  std::cout << "The authority has been destroyed successfully!"
		    << std::endl;

	  break;
	}
      case Authority::OperationInformation:
	{
	  // get information on the authority.
	  if (Authority::Information() == elle::StatusError)
	    escape("unable to retrieve information on the authority");

	  break;
	}
      case Authority::OperationUnknown:
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
