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
// updated       julien quintard   [fri aug 14 13:00:48 2009]
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

  // XXX
  KeyPair PIG::User;
  // XXX

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
    /* XXX
    operations.getattr = PIG::getattr;
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

    // XXX[hack for the /]
    {
      int		fd;

      fd = ::open(".device/.root", O_RDONLY);

      Region	region;

      region.Prepare(4096);

      region.size = ::read(fd, region.contents, region.capacity);

      region.Detach();

      Archive		archive;

      archive.Prepare(region);

      Address		address;

      archive.Extract(address);

      String		root("/");

      if (etoile::path::Path::Initialize(address) == StatusError)
	escape("unable to initialize the path module");
    }

    // XXX[hack for the user keypair]
    {
      int		fd;

      fd = ::open(".device/.user", O_RDONLY);

      Region	region;

      region.Prepare(4096);

      region.size = ::read(fd, region.contents, region.capacity);

      region.Detach();

      Archive		archive;

      archive.Prepare(region);

      archive.Extract(PIG::User);
    }

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

}
