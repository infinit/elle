//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/hole.cc
//
// created       julien quintard   [wed may 11 15:20:51 2011]
// updated       julien quintard   [mon jul 11 22:53:05 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>
#include <hole/Hole.hh>

namespace hole
{

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
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

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // allocate a new parser.
    Infinit::Parser = new elle::Parser(argc, argv);

    // specify a program description.
    if (Infinit::Parser->Description(Infinit::Version +
				     " "+
				     "Copyright (c) 2008, 2009, 2010, 2011, "
				     "Julien Quintard, All rights "
				     "reserved.\n") == elle::StatusError)
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

    // launch the program.
    if (elle::Program::Launch() == elle::StatusError)
      escape("an error occured while processing events");

    // delete the parser.
    delete Infinit::Parser;

    // waive.
    waive(Infinit::Parser);

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

}

//
// ---------- main ------------------------------------------------------------
//

///
/// this is the program entry point.
///
int			main(int				argc,
                             char*				argv[])
{
  try
    {
      if (hole::Main(argc, argv) == elle::StatusError)
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
