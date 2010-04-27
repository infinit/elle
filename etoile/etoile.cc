//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/etoile.cc
//
// created       julien quintard   [wed mar  3 22:36:08 2010]
// updated       julien quintard   [tue apr 27 17:13:07 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/Etoile.hh>

namespace etoile
{

//
// ---------- functions -------------------------------------------------------
//

  Status		Main(const Natural32			argc,
			     const Character*			argv[])
  {
    enter();

    // initialize the Elle library.
    if (Elle::Initialize() == StatusError)
      escape("unable to initialize the Elle library");

    // set up the program.
    if (Program::Setup(argc, argv) == StatusError)
      escape("unable to set up the program");

    // initialize etoile.
    if (Etoile::Initialize() == StatusError)
      escape("unable to initialize etoile");

    // launch the program.
    if (Program::Launch() == StatusError)
      escape("an error occured while processing events");

    // clean etoile.
    if (Etoile::Clean() == StatusError)
      escape("unable to clean etoile");

    // clean the Elle library.
    if (Elle::Clean() == StatusError)
      escape("unable to clean the Elle library");

    leave();
  }

}

//
// ---------- main ------------------------------------------------------------
//

///
/// XXX
///
int			main(const int			argc,
			     const char*		argv[])
{
  etoile::Main(argc, argv);

  expose();

  return (0);
}
