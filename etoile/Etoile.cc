//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/Etoile.cc
//
// created       julien quintard   [wed mar  3 22:36:08 2010]
// updated       julien quintard   [fri mar 19 14:55:48 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/Etoile.hh>

namespace etoile
{

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes Etoile.
  ///
  /// XXX \todo virer le code temporaire + mettre chaque valeur dans la
  /// XXX       configuration (root, pair et path)
  ///
  Status		Etoile::Initialize()
  {
    hole::Address	root;
    String		path;

    enter();

    // XXX
    {
      // XXX
      path = "/home/mycure/.infinit";

      // XXX[hack for the /]
      {
	int		fd;

	fd = ::open("/home/mycure/.infinit/hole/.root", O_RDONLY);

	Region	region;

	region.Prepare(4096);

	region.size = read(fd, region.contents, region.capacity);

	region.Detach();

	Archive		archive;

	archive.Prepare(region);

	archive.Extract(root);

	expose();

	std::cout << "[root directory]" << std::endl;
	root.Dump();
      }
    }

    // initialize the configuration.
    if (Configuration::Initialize(path) == StatusError)
      escape("unable to initialize the configuration");

    // initialize the core.
    if (Core::Initialize() == StatusError)
      escape("unable to initialize the core");

    // initialize the depot.
    if (Depot::Initialize() == StatusError)
      escape("unable to initialize the depot");

    // initialize the journal.
    if (Journal::Initialize() == StatusError)
      escape("unable to initialize the journal");

    // initialize the path.
    if (path::Path::Initialize(root) == StatusError)
      escape("unable to initialize the journal");

    // initialize the wall.
    if (Wall::Initialize() == StatusError)
      escape("unable to initialize the journal");

    leave();
  }

  ///
  /// this method cleans Etoile.
  ///
  Status		Etoile::Clean()
  {
    enter();

    // clean the wall.
    if (Wall::Clean() == StatusError)
      escape("unable to clean the journal");

    // clean the path.
    if (path::Path::Clean() == StatusError)
      escape("unable to clean the journal");

    // clean the journal.
    if (Journal::Clean() == StatusError)
      escape("unable to clean the journal");

    // clean the depot.
    if (Depot::Clean() == StatusError)
      escape("unable to clean the depot");

    // clean the core.
    if (Core::Clean() == StatusError)
      escape("unable to clean the core");

    // clean the configuration.
    if (Configuration::Clean() == StatusError)
      escape("unable to clean the configuration");

    leave();
  }

//
// ---------- functions -------------------------------------------------------
//

  Status		Main(Natural32			argc,
			     Character*			argv[])
  {
    enter();

    // initialize the Elle library.
    if (Elle::Initialize() == StatusError)
      escape("unable to initialize the Elle library");

    // set up the application.
    if (elle::Application::Setup(argc, argv) == StatusError)
      escape("unable to set up the application");

    // initialize etoile.
    if (Etoile::Initialize() == StatusError)
      escape("unable to initialize etoile");

    // process events.
    if (elle::Application::Process() == StatusError)
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
int			main(int			argc,
			     char**			argv)
{
  etoile::Main(argc, argv);

  expose();

  return (0);
}
