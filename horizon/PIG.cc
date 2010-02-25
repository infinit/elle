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
// updated       julien quintard   [sun feb  7 05:04:46 2010]
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

  static struct fuse_operations	operations;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the system module.
  ///
  Status		PIG::Initialize()
  {
    // initialize the cryptographic module.
    if (Cryptography::Initialize() == StatusError)
      escape("unable to initialize the cryptography");

    // set the operations.
    operations.getattr = PIG::getattr;
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

    leave();
  }

  ///
  /// this method cleans the system module.
  ///
  Status		PIG::Clean()
  {
    // clean the cryptographic module.
    if (Cryptography::Clean() == StatusError)
      escape("unable to clean the cryptography");

    leave();
  }

  ///
  /// this is the entry point of the FUSE module.
  ///
  Status		PIG::Main(int				argc,
				  char**			argv)
  {
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
// ---------- interface -------------------------------------------------------
//

  ///
  /// XXX
  ///
  int			PIG::getattr(const char*		path,
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
