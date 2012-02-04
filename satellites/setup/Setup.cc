//
// ---------- header ----------------------------------------------------------
//
// project       setup
//
// license       infinit
//
// author        julien quintard   [thu mar  4 17:51:46 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <satellites/setup/Setup.hh>

namespace satellite
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character         Component[] = "8setup";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the user's environment.
  ///
  elle::Status          Setup::Initialize()
  {
    elle::Path          path;
    lune::Configuration configuration;

    enter();

    //
    // initialize the infinit path.
    //
    {
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
      else
        {
          // clear the directory.
          if (elle::Directory::Clear(path) == elle::StatusError)
            escape("unable to clear the directory");
        }
    }

    //
    // create the configuration file.
    //
    {
      // pull the default parameters.
      if (configuration.Pull() == elle::StatusError)
        escape("unable to pull the configuration parameters");

      // push the current parameters.
      if (configuration.Push() == elle::StatusError)
        escape("unable to pus the parameters");

      // store the configuration.
      if (configuration.Store() == elle::StatusError)
        escape("unable to store the configuration");
    }

    leave();
  }

  ///
  /// this method cleans the user's environment.
  ///
  elle::Status          Setup::Clean()
  {
    elle::Path          path;

    enter();

    //
    // remove the configuration, if necessary.
    //
    {
      lune::Configuration       configuration;

      // if the configuration exists...
      if (configuration.Exist() == elle::StatusTrue)
        {
          // remove it.
          if (configuration.Erase() == elle::StatusError)
            escape("unable to erase the configuration");
        }
    }

    //
    // reinitialize the infinit directory.
    //
    {
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
    }

    leave();
  }

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  elle::Status          Main(elle::Natural32                    argc,
                             elle::Character*                   argv[])
  {
    Setup::Operation    operation;

    enterx(instance(Infinit::Parser));

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
    operation = Setup::OperationUnknown;

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
int                     main(int                                argc,
                             char**                             argv)
{
  try
    {
      if (satellite::Main(argc, argv) == elle::StatusError)
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
