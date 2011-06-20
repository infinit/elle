//
// ---------- header ----------------------------------------------------------
//
// project       8setup
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8setup/Setup.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [sun jun 19 17:46:06 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8setup/Setup.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "8setup";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the user's environment.
  ///
  elle::Status		Setup::Initialize()
  {
    enter();

    // nothing to do.

    leave();
  }

  ///
  /// this method cleans the user's environment.
  ///
  elle::Status		Setup::Clean()
  {
    enter();

    // XXX clean the directory

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
    elle::Parser*	parser;
    Setup::Operation	operation;
    elle::Character	option;

    enter();

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // initialize the operation.
    operation = Setup::OperationUnknown;

    // allocate a new parser.
    parser = new elle::Parser(argc, argv);

    // set up the parser.
    if (parser->Register('h',
			 "help",
			 "display the help",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('i',
			 "initialize",
			 "initialize the user's infinit environment",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('c',
			 "clean",
			 "clean the user's infinit environment",
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
	  case 'i':
	    {
	      // check if the operation has already been set up.
	      if (operation != Setup::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the initialize operation cannot be set concurrently "
			 "to another operation");
		}

	      operation = Setup::OperationInitialize;

	      break;
	    }
	  case 'c':
	    {
	      // check if the operation has already been set up.
	      if (operation != Setup::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the clean operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Setup::OperationClean;

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
      case Setup::OperationInitialize:
	{
	  // initialize the environment.
	  if (Setup::Initialize() == elle::StatusError)
	    escape("unable to initialize the environment");

	  // display a message.
	  std::cout << "The infinit environment has been "
		    << "initialized successfully!"
		    << std::endl;

	  break;
	}
      case Setup::OperationClean:
	{
	  // clean the environment.
	  if (Setup::Clean() == elle::StatusError)
	    escape("unable to clean the environment");

	  // display a message.
	  std::cout << "The infinit environment has been "
		    << "cleaned successfully!"
		    << std::endl;

	  break;
	}
      case Setup::OperationUnknown:
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

    // clean Lune.
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
