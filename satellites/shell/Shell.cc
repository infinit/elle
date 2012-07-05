#include <satellites/shell/Shell.hh>

#include <elle/Elle.hh>
#include <elle/utility/Parser.hh>
#include <elle/concurrency/Program.hh>

#include <nucleus/Nucleus.hh>

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
  nucleus::proton::Address Shell::Address;

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

    return elle::Status::Ok;
  }

  ///
  /// this command quits the shell.
  ///
  elle::Status          Shell::Quit(const elle::String&)
  {
    // exit.
    ::exit(0);

    return elle::Status::Ok;
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

    return elle::Status::Ok;
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
    if (elle::Elle::Initialize() == elle::Status::Error)
      escape("unable to initialize Elle");

    // set up the program.
    if (elle::concurrency::Program::Setup() == elle::Status::Error)
      escape("unable to set up the program");

    // initialize the nucleus library.
    if (nucleus::Nucleus::Initialize() == elle::Status::Error)
      escape("unable to initialize Nucleus");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::Status::Error)
      escape("unable to initialize Lune");

    // initialize Infinit.
    if (Infinit::Initialize() == elle::Status::Error)
      escape("unable to initialize Infinit");

    // initialize the Etoile library.
    if (etoile::Etoile::Initialize() == elle::Status::Error)
      escape("unable to initialize Etoile");

    // allocate a new parser.
    Infinit::Parser = new elle::utility::Parser(argc, argv);

    // specify a program description.
    if (Infinit::Parser->Description(Infinit::Copyright) == elle::Status::Error)
      escape("unable to set the description");

    // register the options.
    if (Infinit::Parser->Register(
          "Help",
          'h',
          "help",
          "display the help",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
          "User",
          'u',
          "user",
          "specifies the name of the user",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
          "Network",
          'n',
          "network",
          "specifies the name of the network",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // parse.
    if (Infinit::Parser->Parse() == elle::Status::Error)
      escape("unable to parse the command line");

    // test the option.
    if (Infinit::Parser->Test("Help") == elle::Status::True)
      {
        // display the usage.
        Infinit::Parser->Usage();

        // quit.
        return elle::Status::Ok;
      }

    // retrieve the user name.
    if (Infinit::Parser->Value("User",
                               Infinit::User) == elle::Status::Error)
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("unable to retrieve the user name");
      }

    // retrieve the network name.
    if (Infinit::Parser->Value("Network",
                               Infinit::Network) == elle::Status::Error)
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("unable to retrieve the network name");
      }

    // initialize the Agent library.
    if (agent::Agent::Initialize() == elle::Status::Error)
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
                if (Commands[i].function(line) == elle::Status::Error)
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
    if (hole::Hole::Clean() == elle::Status::Error)
      escape("unable to clean Hole");

    // clean the Agent library.
    if (agent::Agent::Clean() == elle::Status::Error)
      escape("unable to clean Agent");

    // clean the Etoile library.
    if (etoile::Etoile::Clean() == elle::Status::Error)
      escape("unable to clean Etoile");

    // clean Infinit.
    if (Infinit::Clean() == elle::Status::Error)
      escape("unable to clean Infinit");

    // clean Lune
    if (lune::Lune::Clean() == elle::Status::Error)
      escape("unable to clean Lune");

    // clean the nucleus library.
    if (nucleus::Nucleus::Clean() == elle::Status::Error)
      escape("unable to clean Nucleus");

    // clean Elle.
    if (elle::Elle::Clean() == elle::Status::Error)
      escape("unable to clean Elle");

    return elle::Status::Ok;
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
      if (satellite::Main(argc, argv) == elle::Status::Error)
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
