//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/system/fuse/Interface.cc
//
// created       julien quintard   [fri jul 31 22:47:18 2009]
// updated       julien quintard   [mon aug  3 21:17:44 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/system/fuse/Interface.hh>

// XXX
#define issue(_errno_)							\
  {									\
    show();								\
									\
    return (errno == 0 ? -_errno_ : -errno);				\
  }
// XXX

namespace pig
{
  namespace system
  {

    // XXX
    //  for every syscall, check the manpages and the ERRORS section.
    //  our implementation should returns these errors, hence all the
    //  cases must be handled.
    // XXX
    //  le code devrait etre splitte en fichier genre File, Directory,
    //  Attributes etc. car c'est un peu le bordel
    // XXX

//
// ---------- definitions -----------------------------------------------------
//

    // XXX[temporary]
    KeyPair		Interface::User;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// these methods represent the FUSE interface.
    ///

    int			Interface::getattr(const char*		path,
					   struct stat*		stat)
    {
      etoile::core::Object	object;
      Address			address;

      printf("[XXX] %s(%s, 0x%x)\n",
	     __PRETTY_FUNCTION__,
	     path, stat);

      // clear the stat structure.
      ::memset(stat, 0x0, sizeof(struct stat));

      // resolve the path.
      if (Path::Resolve(path, address) == StatusError)
	issue(ENOENT);

      // retrieve the object.
      if (Hole::Get(address, object) == StatusError)
	issue(ENOENT);

      // fill the stat structure.

      // XXX[here, for the perms, the others perms should be set according
      //     to the presence of delegates/consumers]
      switch (object.meta.status.type)
	{
	case etoile::core::Object::TypeFile:
	  {
	    stat->st_mode = S_IFREG | 0600;
	    break;
	  }
	case etoile::core::Object::TypeDirectory:
	  {
	    stat->st_mode = S_IFDIR | 0700;
	    break;
	  }
	}

      stat->st_uid = getuid();
      stat->st_gid = getgid();

      stat->st_size = object.meta.status.size;

      // XXX[the dates must be properly converted and filled]
      stat->st_atime = time(NULL);
      stat->st_mtime = time(NULL);
      stat->st_ctime = time(NULL);

      return (0);
    }

    int			Interface::access(const char*		path,
					  int			mask)
    {
      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      return (0);
    }

    int			Interface::mknod(const char*		path,
					 mode_t			mode,
					 dev_t			dev)
    {
      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      return (0);
    }

    int			Interface::unlink(const char*		path)
    {
      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      return (0);
    }

    int			Interface::utimens(const char*		path,
					   const struct timespec tv[2])
    {
      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      return (0);
    }

    int			Interface::mkdir(const char*		path,
					 mode_t			mode)
    {
      etoile::core::Directory	parent;
      etoile::core::Directory	child;
      Address			address;
      String			route(path);
      String			directory;
      String			name;

      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      // XXX check if it already exists, check the perms etc.

      // retrieve the base and dir names.
      if (Path::Break(route, "/", directory, name) == StatusError)
	issue(ECANCELED);

      // resolve the parent directory path.
      if (Path::Resolve(directory, address) == StatusError)
	issue(ENOENT);

      // retrieve the parent directory.
      if (Hole::Get(address, parent) == StatusError)
	issue(ENOENT);

      // create a child directory.
      if (Directory::Create(child, Interface::User) == StatusError)
	issue(ECANCELED);

      // store the child directory.
      if (Directory::Store(child) == StatusError)
	issue(ECANCELED);

      // add an entry in the parent directory.
      if (Directory::Add(parent, name, child.address,
			 Interface::User.k) == StatusError)
	issue(ECANCELED);

      // store the parent directory.
      if (Directory::Store(parent) == StatusError)
	issue(ECANCELED);

      return (0);
    }

    int			Interface::readdir(const char*		path,
					   void*		buffer,
					   fuse_fill_dir_t	filler,
					   off_t		offset,
					   struct fuse_file_info* info)
    {
      etoile::core::Directory		directory;
      etoile::core::Catalog		catalog;
      etoile::core::Catalog::Iterator	iterator;
      Address				address;

      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      // resolve the path.
      if (Path::Resolve(path, address) == StatusError)
	issue(ENOENT);

      // retrieve the directory.
      if (Hole::Get(address, directory) == StatusError)
	issue(ENOENT);

      // XXX[these . and .. entries are probably UNIX-specific]
      // fill the . and .. entries.
      filler(buffer, ".", NULL, 0);
      filler(buffer, "..", NULL, 0);

      // if no catalog, just return since there is no entry.
      if (directory.data.references == Address::Null)
	return (0);

      // load the catalog.
      if (Hole::Get(directory.data.references, catalog) == StatusError)
	issue(ENOENT);

      // XXX[this implementation does not handle offset but definitely should!]
      // XXX[besides this implementation is intrusive!!!]

      // walk through the catalog.
      for (iterator = catalog.entries.begin();
	   iterator != catalog.entries.end();
	   iterator++)
	{
	  Catalog::Entry*	entry = *iterator;

	  filler(buffer, entry->name.c_str(), NULL, 0);
	}

      return (0);
    }

    int			Interface::rmdir(const char*		path)
    {
      etoile::core::Directory	parent;
      etoile::core::Directory	child;
      Address			address;
      String			route(path);
      String			directory;
      String			name;
      Natural32			size;

      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      // XXX check the perms etc.

      // resolve the directory path.
      if (Path::Resolve(path, address) == StatusError)
	issue(ENOENT);

      // load the target directory.
      if (Hole::Get(address, child) == StatusError)
	issue(ENOENT);

      // check if the directory is empty.
      if (Directory::Size(child, size) == StatusError)
	issue(ECANCELED);

      if (size != 0)
	issue(ENOTEMPTY);

      // destroy the directory block.
      if (Hole::Destroy(address) == StatusError)
	issue(ECANCELED);

      // retrieve the base and dir names.
      if (Path::Break(route, "/", directory, name) == StatusError)
	issue(ECANCELED);

      // resolve the parent directory path.
      if (Path::Resolve(directory, address) == StatusError)
	issue(ENOENT);

      // retrieve the parent directory.
      if (Hole::Get(address, parent) == StatusError)
	issue(ENOENT);

      // delete the entry.
      if (Directory::Remove(parent, name, Interface::User.k) == StatusError)
	issue(ECANCELED);

      // store the parent directory.
      if (Directory::Store(parent) == StatusError)
	issue(ECANCELED);

      return (0);
    }

  }
}
