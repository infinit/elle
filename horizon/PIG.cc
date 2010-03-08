//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/PIG.cc
//
// created       julien quintard   [fri jul 31 22:10:21 2009]
// updated       julien quintard   [thu mar  4 13:10:03 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/PIG.hh>

namespace pig
{

//
// ---------- globals ---------------------------------------------------------
//

  ///
  /// XXX
  ///
  static struct fuse_operations	operations;

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// this is the entry point of the FUSE module.
  ///
  Status		Main(Natural32				argc,
			     Character*				argv[])
  {
    enter();

    // initialize the module.
    if (PIG::Initialize() == StatusError)
      escape("unable to initialize the module");

    // launch fuse.
    if (fuse_main(argc, argv, &operations, NULL) != 0)
      escape("an error occured in FUSE");

    // clean the module.
    if (PIG::Clean() == StatusError)
      escape("unable to clean the module");

    leave();
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the system module.
  ///
  Status		PIG::Initialize()
  {
    enter();

    // initialize the Elle library.
    if (Elle::Initialize() == StatusError)
      escape("unable to initialize the Elle library");

    //
    // set the operations.
    //
    {
      operations.getattr = PIG::Getattr;
      /*
	operations.setxattr = PIG::setxattr;
	operations.chmod = PIG::chmod;
	operations.chown = PIG::chown;

	operations.access = PIG::access;
	operations.utimens = PIG::utimens;

	operations.mknod = PIG::mknod;
	operations.open = PIG::open;
	operations.read = PIG::read;
	operations.write = PIG::write;
	operations.truncate = PIG::truncate;
	operations.release = PIG::release;
	operations.unlink = PIG::unlink;
	operations.rename = PIG::rename;
	operations.readlink = PIG::readlink;

	operations.symlink = PIG::symlink;

	operations.mkdir = PIG::mkdir;
	operations.readdir = PIG::readdir;
	operations.rmdir = PIG::rmdir;
      */
    }

    //
    // connect to Etoile.
    //
    {
      static Function<String>	challenge(&PIG::Challenge);

      // register the message.
      if (Network::Register<
    }

    leave();
  }

  ///
  /// this method cleans the system module.
  ///
  Status		PIG::Clean()
  {
    enter();

    // clean the Elle library.
    if (Elle::Clean() == StatusError)
      escape("unable to clean the Elle library");

    leave();
  }

//
// ---------- interface -------------------------------------------------------
//

  ///
  /// XXX
  ///
  int			PIG::Getattr(const char*		path,
				     struct stat*		stat)
  {
    printf("[XXX] %s(%s, 0x%x)\n",
	   __FUNCTION__,
	   path, stat);

    // clear the stat structure.
    ::memset(stat, 0x0, sizeof(struct stat));

    // issue a Request(Input, Output)

    stat->st_uid = getuid();
    stat->st_gid = getgid();

    // XXX stat->st_size = object.meta.status.size;

    // XXX[the dates must be properly converted and filled]
    stat->st_atime = time(NULL);
    stat->st_mtime = time(NULL);
    stat->st_ctime = time(NULL);

    return (0);
  }

}

//
// ---------- main ------------------------------------------------------------
//

int			main(int			argc,
			     char**			argv)
{
  pig::Main(argc, argv);

  expose();

  return (0);
}
