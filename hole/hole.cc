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
// updated       julien quintard   [wed may 11 17:26:21 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Hole.hh>

#include <lune/Lune.hh>

namespace hole
{

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  elle::Status		Main(const elle::Natural32		argc,
			     const elle::Character*		argv[])
  {
    enter();

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize the Elle library");

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // initialize the nucleus library.
    if (nucleus::Nucleus::Initialize() == elle::StatusError)
      escape("unable to initialize Nucleus");

    // initialize the lune library.
    if (lune::Lune::Initialize() == elle::StatusError)
      escape("unable to initialize the Lune library");

    // initialize the hole.
    if (Hole::Initialize() == elle::StatusError)
      escape("unable to initialize the hole");

    // launch the program.
    if (elle::Program::Launch() == elle::StatusError)
      escape("unable to process the events");

    // clean the hole.
    if (Hole::Clean() == elle::StatusError)
      escape("unable to clean the hole");

    // clean the Lune library.
    if (lune::Lune::Clean() == elle::StatusError)
      escape("unable to clean the Lune library");

    // clean the nucleus library.
    if (nucleus::Nucleus::Clean() == elle::StatusError)
      escape("unable to clean Nucleus");

    // clean the Elle library.
    if (elle::Elle::Clean() == elle::StatusError)
      escape("unable to clean the Elle library");

    leave();
  }

}

//
// ---------- main ------------------------------------------------------------
//

///
/// this is the program entry point.
///
int			main(const int				argc,
                             const char*			argv[])
{
  hole::Main(argc, argv);

  expose();

  return (0);
}
