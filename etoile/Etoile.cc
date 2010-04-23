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
// updated       julien quintard   [thu apr 22 12:41:34 2010]
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

	std::cout << "[root] " << root << std::endl;
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

}
