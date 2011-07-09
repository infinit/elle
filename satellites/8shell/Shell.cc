//
// ---------- header ----------------------------------------------------------
//
// project       8debug
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8shell/Shell.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [sat jul  9 19:20:19 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8shell/Shell.hh>

#include <lune/Lune.hh>
#include <etoile/Etoile.hh>
#include <agent/Agent.hh>
#include <hole/Hole.hh>
#include <Infinit.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "8shell";

  ///
  /// this variable contains the address of the current object.
  ///
  nucleus::Address		Shell::Address;

  ///
  /// the shell commands.
  ///
  Shell::Command		Commands[] =
    {
      /*
      { "enter", Shell::Enter },
      { "list", Shell::List },
      { "select", Shell::Select },
      { "location", Shell::Location },
      */
      { "dump", Shell::Dump },
      { "help", Shell::Help },
      { "quit", Shell::Quit },
      { NULL, NULL }
    };

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// XXX
  ///
  elle::Status		Shell::Dump(const elle::String&		command)
  {
    enter();

    std::cout << command << std::endl;

    leave();
  }

  ///
  /// XXX
  ///
  elle::Status		Shell::Help(const elle::String&)
  {
    elle::Natural32	i;

    enter();

    std::cout << "Commands:" << std::endl;

    for (i = 0; Commands[i].name; i++)
      std::cout << "  " << Commands[i].name << std::endl;

    leave();
  }

  ///
  /// XXX
  ///
  elle::Status		Shell::Quit(const elle::String&)
  {
    enter();

    // exit.
    ::exit(0);

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
    elle::Character*	line;

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

    // set up the agent-specific options.
    if (agent::Agent::Options() == elle::StatusError)
      escape("unable to set up the options");

    // set up the hole-specific options.
    if (hole::Hole::Options() == elle::StatusError)
      escape("unable to set up the options");

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

    // initialize the Agent library.
    if (agent::Agent::Initialize() == elle::StatusError)
      escape("unable to initialize Agent");

    // initialize the Hole library.
    if (hole::Hole::Initialize() == elle::StatusError)
      escape("unable to initialize Hole");

    // wait for and trigger commands.
    while ((line = ::readline("$> ")) != NULL)
      {
	elle::Natural32	i;

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
