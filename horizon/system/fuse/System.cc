//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/system/fuse/System.cc
//
// created       julien quintard   [fri jul 31 22:10:21 2009]
// updated       julien quintard   [sat aug  1 20:59:54 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/system/fuse/System.hh>

// XXX
using namespace etoile::core;
// XXX

namespace pig
{
  namespace system
  {

//
// ---------- globals ---------------------------------------------------------
//

    static struct fuse_operations	operations;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the system module.
    ///
    Status		System::Initialize()
    {
      // initialize the cryptographic module.
      if (Cryptography::Initialize() == StatusError)
	escape("unable to initialize the cryptography");

      // set the operations.
      operations.getattr = Interface::getattr;
      operations.access = Interface::access;
      operations.mknod = Interface::mknod;
      operations.unlink = Interface::unlink;
      operations.utimens = Interface::utimens;
      operations.readdir = Interface::readdir;
      operations.mkdir = Interface::mkdir;

      // XXX[hack for the /]
      {
	int		fd;

	fd = ::open(".device/.root", O_RDONLY);

	Region	region;

	region.Prepare(4096);

	region.size = read(fd, region.contents, region.capacity);

	region.Detach();

	Archive		archive;

	archive.Prepare(region);

	Address		address;

	archive.Extract(address);

	String		root("/");

	if (Cache::Put(root, address, Cache::TypePermanent) == StatusError)
	  escape("unable to record the '/' path in the cache");
      }

      leave();
    }

    ///
    /// this method cleans the system module.
    ///
    Status		System::Clean()
    {
      // clean the cryptographic module.
      if (Cryptography::Clean() == StatusError)
	escape("unable to clean the cryptography");

      leave();
    }

    ///
    /// this is the entry point of the FUSE module.
    ///
    Status		System::Main(int		argc,
				     char**		argv)
    {
      // initialize the module.
      if (System::Initialize() == StatusError)
	escape("unable to initialize the module");

      // launch fuse.
      if (fuse_main(argc, argv, &operations, NULL) != 0)
	escape("an error occured in FUSE");

      // clean the module.
      if (System::Clean() == StatusError)
	escape("unable to clean the module");

      leave();
    }

  }
}
