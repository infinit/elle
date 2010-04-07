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
// updated       julien quintard   [wed mar 31 21:38:56 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/Etoile.hh>

namespace etoile
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const Character		Component[] = "etoile";

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
    if (configuration::Configuration::Initialize(path) == StatusError)
      escape("unable to initialize the configuration");

    // initialize the kernel.
    if (kernel::Kernel::Initialize() == StatusError)
      escape("unable to initialize the kernel");

    // initialize the depot.
    if (depot::Depot::Initialize() == StatusError)
      escape("unable to initialize the depot");

    // initialize the journal.
    if (journal::Journal::Initialize() == StatusError)
      escape("unable to initialize the journal");

    // initialize the path.
    if (path::Path::Initialize(root) == StatusError)
      escape("unable to initialize the path");

    // initialize the wall.
    if (wall::Wall::Initialize() == StatusError)
      escape("unable to initialize the wall");

    // initialize the user.
    if (user::User::Initialize() == StatusError)
      escape("unable to initialize the user");

    leave();
  }

  ///
  /// this method cleans Etoile.
  ///
  Status		Etoile::Clean()
  {
    enter();

    // clean the user.
    if (user::User::Clean() == StatusError)
      escape("unable to clean the user");

    // clean the wall.
    if (wall::Wall::Clean() == StatusError)
      escape("unable to clean the wall");

    // clean the path.
    if (path::Path::Clean() == StatusError)
      escape("unable to clean the path");

    // clean the journal.
    if (journal::Journal::Clean() == StatusError)
      escape("unable to clean the journal");

    // clean the depot.
    if (depot::Depot::Clean() == StatusError)
      escape("unable to clean the depot");

    // clean the kernel.
    if (kernel::Kernel::Clean() == StatusError)
      escape("unable to clean the kernel");

    // clean the configuration.
    if (configuration::Configuration::Clean() == StatusError)
      escape("unable to clean the configuration");

    leave();
  }

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
