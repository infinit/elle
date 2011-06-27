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
// updated       julien quintard   [mon jun 27 22:12:51 2011]
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
    elle::Path	path;

    enter();

    // create the home path.
    if (path.Create(lune::Lune::Home) == elle::StatusError)
      escape("unable to create the path");

    // if the user directory does not exist, create it.
    if (elle::Directory::Exist(path) == elle::StatusFalse)
      {
	// create the directory.
	if (elle::Directory::Create(path) == elle::StatusError)
	  escape("unable to create the directory");
      }

    leave();
  }

  ///
  /// this method cleans the user's environment.
  ///
  elle::Status		Setup::Clean()
  {
    elle::Path	path;

    enter();

    // create the home path.
    if (path.Create(lune::Lune::Home) == elle::StatusError)
      escape("unable to create the path");

    // if the user directory exists, clear it and remove it.
    if (elle::Directory::Exist(path) == elle::StatusTrue)
      {
	// clear the content.
	if (elle::Directory::Clear(path) == elle::StatusError)
	  escape("unable to clear the directory");

	// remove the directory.
	if (elle::Directory::Remove(path) == elle::StatusError)
	  escape("unable to remove the directory");
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
    Setup::Operation	operation;

    enter(instance(Infinit::Parser));

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize Elle");

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
    operation = Setup::OperationUnknown;

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

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
          "Initialize",
	  'i',
	  "initialize",
	  "initialize the user's infinit environment",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Clean",
	  'c',
	  "clean",
	  "clean the user's infinit environment",
	  elle::Parser::KindNone) == elle::StatusError)
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
    if ((Infinit::Parser->Test("Initialize") == elle::StatusTrue) &&
	(Infinit::Parser->Test("Clean") == elle::StatusTrue))
      {
	// display the usage.
	Infinit::Parser->Usage();

	escape("the initialize and clean options are mutually exclusive");
      }

    // test the option.
    if (Infinit::Parser->Test("Initialize") == elle::StatusTrue)
      operation = Setup::OperationInitialize;

    // test the option.
    if (Infinit::Parser->Test("Clean") == elle::StatusTrue)
      operation = Setup::OperationClean;

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
	  Infinit::Parser->Usage();

	  escape("please specify an operation to perform");
	}
      }

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

    // delete the parser.
    delete Infinit::Parser;

    // waive.
    waive(Infinit::Parser);

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
