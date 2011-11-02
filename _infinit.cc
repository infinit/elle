//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// author        julien quintard   [wed jun  1 10:53:21 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>
#include <agent/Agent.hh>
#include <etoile/Etoile.hh>
#include <hole/Hole.hh>

#if INFINIT_UNIX
# include <pig/PIG.hh>
#elif INFINIT_WIN32
# include <iig/IIG.hh>
#endif

#include <elle/idiom/Close.hh>
# include <exception>
#include <elle/idiom/Open.hh>

//
// ---------- functions -------------------------------------------------------
//

///
/// this is the entry point of pig.
///
elle::Status		Main(elle::Natural32			argc,
                             elle::Character*			argv[])
{
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

  // set up the hole-specific options.
  if (hole::Hole::Options() == elle::StatusError)
    escape("unable to set up the options");

  // set up the agent-specific options.
  if (agent::Agent::Options() == elle::StatusError)
    escape("unable to set up the options");

#if INFINIT_UNIX
  // set up the pig-specific options.
  if (pig::PIG::Options() == elle::StatusError)
    escape("unable to set up the options");
#elif INFINIT_WIN32
  // set up the iig-specifi options.
  if (iig::IIG::Options() == elle::StatusError)
    escape("unable to set up iig options");
#endif

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

  // initialize the Hole library.
  if (hole::Hole::Initialize() == elle::StatusError)
    escape("unable to initialize Hole");

  // initialize the Agent library.
  if (agent::Agent::Initialize() == elle::StatusError)
    escape("unable to initialize Agent");

  // initialize the Etoile library.
  if (etoile::Etoile::Initialize() == elle::StatusError)
    escape("unable to initialize Etoile");

#if INFINIT_UNIX
  // initialize PIG.
  if (pig::PIG::Initialize() == elle::StatusError)
    escape("unable to initialize PIG");
#elif INFINIT_WIN32
  // initialize IIG.
  if (iig::IIG::Initialize() == elle::StatusError)
    escape("unable to initialize IIG");
#endif

  // launch the program.
  if (elle::Program::Launch() == elle::StatusError)
    escape("an error occured while processing events");

  // delete the parser.
  delete Infinit::Parser;

  // waive.
  waive(Infinit::Parser);

#if INFINIT_UNIX
  // clean PIG.
  if (pig::PIG::Clean() == elle::StatusError)
    escape("unable to clean PIG");
#elif INFINIT_WIN32
  // clean IIG.
  if (iig::IIG::Clean() == elle::StatusError)
    escape("unable to clean IIG");
#endif

  // clean the Etoile library.
  if (etoile::Etoile::Clean() == elle::StatusError)
    escape("unable to clean Etoile");

  // clean the Agent library.
  if (agent::Agent::Clean() == elle::StatusError)
    escape("unable to clean Agent");

  // clean Hole.
  if (hole::Hole::Clean() == elle::StatusError)
    escape("unable to clean Hole");

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

// XXX
static void terminate_handler()
{
  std::cerr << "uncaught excpection";
  try {
    throw;
  }
  catch (std::exception & e) {
    std::cerr << ": `" << e.what() << "'" << std::endl;
  }
  catch (...) {
    std::cerr << std::endl;
  }

  // XXX print the back trace
  _exit(1);
}

//
// ---------- main ------------------------------------------------------------
//

int			main(int				argc,
                             char*				argv[])
{
  std::set_terminate(terminate_handler);

  try
    {
      if (Main(argc, argv) == elle::StatusError)
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
