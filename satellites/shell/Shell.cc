#include <satellites/shell/Shell.hh>

#include <lune/Lune.hh>
#include <etoile/Etoile.hh>
#include <agent/Agent.hh>
#include <hole/Hole.hh>
#include <Infinit.hh>

namespace satellite
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character         Component[] = "8shell";

  ///
  /// this variable contains the address of the current object.
  ///
  nucleus::Address              Shell::Address;

  ///
  /// the shell commands.
  ///
  Shell::Command                Commands[] =
    {
      { "help", Shell::Help },
      { "quit", Shell::Quit },
      { "dump", Shell::Dump },
      { NULL, NULL }
    };

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this command displays the help.
  ///
  elle::Status          Shell::Help(const elle::String&)
  {
    elle::Natural32     i;

    std::cout << "Commands:" << std::endl;

    for (i = 0; Commands[i].name; i++)
      std::cout << "  " << Commands[i].name << std::endl;

    return elle::StatusOk;
  }

  ///
  /// this command quits the shell.
  ///
  elle::Status          Shell::Quit(const elle::String&)
  {
    // exit.
    ::exit(0);

    return elle::StatusOk;
  }

  ///
  /// this command dumps a block given its address
  ///
  elle::Status          Shell::Dump(const elle::String&         line)
  {
    std::istringstream  iss(line);
    elle::String        command;
    elle::String        address;

    // XXX XXX -> ameliorer Parser

    // ignore the command.
    iss >> command;

    // retrieve the address.
    iss >> address;

    // XXX
    std::cout << address << std::endl;

    return elle::StatusOk;
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
    elle::Character*    line;

    // XXX Infinit::Parser is not deleted in case of errors

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

    // register the option.
    if (Infinit::Parser->Register(
          "User",
          'u',
          "user",
          "specifies the name of the user",
          elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
          "Network",
          'n',
          "network",
          "specifies the name of the network",
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
        return elle::StatusOk;
      }

    // retrieve the user name.
    if (Infinit::Parser->Value("User",
                               Infinit::User) == elle::StatusError)
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("unable to retrieve the user name");
      }

    // retrieve the network name.
    if (Infinit::Parser->Value("Network",
                               Infinit::Network) == elle::StatusError)
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("unable to retrieve the network name");
      }

    // initialize the Agent library.
    if (agent::Agent::Initialize() == elle::StatusError)
      escape("unable to initialize Agent");

    hole::Hole::Initialize();

    // wait for and trigger commands.
    while ((line = ::readline("$> ")) != NULL)
      {
        elle::Natural32 i;

        // look for the command.
        for (i = 0; Commands[i].name != NULL; i++)
          {
            // trigger the command.
            if (elle::String(Commands[i].name) == elle::String(line))
              {
                if (Commands[i].function(line) == elle::StatusError)
                  show();

                break;
              }
          }

        // no command found.
        if (Commands[i].name == NULL)
          std::cerr << "Unknown command '" << line << "'" << std::endl;
      }

    // delete the parser.
    delete Infinit::Parser;
    Infinit::Parser = nullptr;

    // clean Hole.
    if (hole::Hole::Clean() == elle::StatusError)
      escape("unable to clean Hole");

    // clean the Agent library.
    if (agent::Agent::Clean() == elle::StatusError)
      escape("unable to clean Agent");

    // clean the Etoile library.
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

    return elle::StatusOk;
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
