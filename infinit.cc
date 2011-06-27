//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/infinit.cc
//
// created       julien quintard   [wed jun  1 10:53:21 2011]
// updated       julien quintard   [mon jun 27 07:49:17 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>

#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>
#include <agent/Agent.hh>
#include <etoile/Etoile.hh>
#include <hole/Hole.hh>
#include <pig/PIG.hh>

//
// ---------- functions -------------------------------------------------------
//

///
/// this is the entry point of pig.
///
elle::Status		Main(elle::Natural32			argc,
			     elle::Character*			argv[])
{
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
	elle::Parser::FormatNone) == elle::StatusError)
    escape("unable to register the option");

  // set up the agent-specific options.
  if (agent::Agent::Options() == elle::StatusError)
    escape("unable to set up the options");

  // set up the hole-specific options.
  if (hole::Hole::Options() == elle::StatusError)
    escape("unable to set up the options");

  // set up the pig-specific options.
  if (pig::PIG::Options() == elle::StatusError)
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

  // initialize PIG.
  if (pig::PIG::Initialize() == elle::StatusError)
    escape("unable to initialize PIG");

  // launch the program.
  if (elle::Program::Launch() == elle::StatusError)
    escape("an error occured while processing events");

  // clean PIG.
  if (pig::PIG::Clean() == elle::StatusError)
    escape("unable to clean PIG");

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

  // delete the parser.
  delete Infinit::Parser;

  // waive.
  waive(Infinit::Parser);

  leave();
}

//
// ---------- main ------------------------------------------------------------
//

int			main(int				argc,
			     char*				argv[])
{
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
