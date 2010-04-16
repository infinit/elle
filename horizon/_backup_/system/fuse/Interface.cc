//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/_backup_/system/fuse/Interface.cc
//
// created       julien quintard   [fri jul 31 22:47:18 2009]
// updated       julien quintard   [thu apr 15 19:27:30 2010]
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
    return (-_errno_);							\
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
    //  replace mknod with creat
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
	     __FUNCTION__,
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
      switch (object.meta.status.component)
	{
	case ComponentFile:
	  {
	    // XXX[here 0600 was changed in 0700 to give all files the X bit]
	    stat->st_mode = S_IFREG | 0700;
	    break;
	  }
	case ComponentDirectory:
	  {
	    stat->st_mode = S_IFDIR | 0700;
	    break;
	  }
	case ComponentLink:
	  {
	    stat->st_mode = S_IFLNK | 0700;
	    break;
	  }
	default:
	  {
	    issue(ENOENT);
	  }
	}

      stat->st_uid = getuid();
      stat->st_gid = getgid();

      // XXX stat->st_size = object.meta.status.size;

      // XXX[the dates must be properly converted and filled]
      stat->st_atime = time(NULL);
      stat->st_mtime = time(NULL);
      stat->st_ctime = time(NULL);

      return (0);
    }

    int			Interface::setxattr(const char*		path,
					    const char*		name,
					    const char*		value,
					    size_t		size,
					    int			flags)
    {
      printf("[XXX] %s(...)\n",
	     __FUNCTION__);

      // XXX

      return (0);
    }

    int			Interface::chmod(const char*		path,
					 mode_t			mode)
    {
      printf("[XXX] %s(%s, 0x%x)\n",
	     __FUNCTION__,
	     path, mode);

      // XXX

      return (0);
    }

    int			Interface::chown(const char*		path,
					 uid_t			uid,
					 gid_t			gid)
    {
      printf("[XXX] %s(%s, %u, %u)\n",
	     __PRETTY_FUNCTION__,
	     path, uid, gid);

      // XXX

      return (0);
    }

    int			Interface::access(const char*		path,
					  int			mask)
    {
      printf("[XXX] %s(%s, 0x%x)\n",
	     __FUNCTION__,
	     path, mask);

      return (0);
    }

    int			Interface::utimens(const char*		path,
					   const struct timespec tv[2])
    {
      printf("[XXX] %s(%s, ...)\n",
	     __PRETTY_FUNCTION__,
	     path);

      return (0);
    }

    int			Interface::mknod(const char*		path,
					 mode_t			mode,
					 dev_t			dev)
    {
      etoile::core::Object	parent;
      etoile::core::Object	child;
      Address			address;
      String			directory;
      String			name;
      Address			self;

      printf("[XXX] %s(%s, 0x%x, %u)\n",
	     __FUNCTION__,
	     path, mode, dev);

      // XXX check if it already exists, check the perms etc.

      // retrieve the base and dir names.
      if (Path::Break(path, "/", directory, name) == StatusError)
	issue(ECANCELED);

      // resolve the parent directory path.
      if (Path::Resolve(directory, address) == StatusError)
	issue(ENOENT);

      // retrieve the parent directory.
      if (Hole::Get(address, parent) == StatusError)
	issue(ENOENT);

      // create a child file.
      if (File::Create(child, Interface::User) == StatusError)
	issue(ECANCELED);

      // store the child file.
      if (File::Store(child) == StatusError)
	issue(ECANCELED);

      // retrieve the address.
      if (child.Self(self) == StatusError)
	issue(ECANCELED);

      // add an entry in the parent directory.
      if (Directory::Add(parent, name, self,
			 Interface::User.k) == StatusError)
	issue(ECANCELED);

      // store the parent directory.
      if (Directory::Store(parent) == StatusError)
	issue(ECANCELED);

      return (0);
    }

    int			Interface::open(const char*		path,
					struct fuse_file_info*	info)
    {
      printf("[XXX] %s(%s, 0x%x)\n",
	     __FUNCTION__,
	     path, info);

      // XXX check perms, optionally set a file handle in the info struct.

      return (0);
    }

    int			Interface::read(const char*		path,
					char*			buffer,
					size_t			size,
					off_t			offset,
					struct fuse_file_info*	info)
    {
      etoile::core::Object	file;
      Address			address;
      Natural64			length = size;

      printf("[XXX] %s(%s, 0x%x, %u, %u, 0x%x)\n",
	     __FUNCTION__,
	     path, buffer, size, offset, info);

      // XXX check if it exists, check the perms etc.

      // resolve the path.
      if (Path::Resolve(path, address) == StatusError)
	issue(ENOENT);

      // retrieve the file.
      if (Hole::Get(address, file) == StatusError)
	issue(ENOENT);

      // read the file.
      if (File::Read(file, (Natural64)offset, (Byte*)buffer, length,
		     Interface::User.k) == StatusError)
	issue(ECANCELED);

      return (length);
    }

    int			Interface::write(const char*		path,
					 const char*		buffer,
					 size_t			size,
					 off_t			offset,
					 struct fuse_file_info*	info)
    {
      etoile::core::Object	file;
      Address			address;

      printf("[XXX] %s(%s, 0x%x, %u, %u, 0x%x)\n",
	     __FUNCTION__,
	     path, buffer, size, offset, info);

      // XXX check if it exists, check the perms etc.

      // resolve the path.
      if (Path::Resolve(path, address) == StatusError)
	issue(ENOENT);

      // retrieve the file.
      if (Hole::Get(address, file) == StatusError)
	issue(ENOENT);

      // write the file.
      if (File::Write(file, offset, (Byte*)buffer, size,
		      Interface::User.k) == StatusError)
	issue(ECANCELED);

      // store the updated file.
      if (File::Store(file) == StatusError)
	issue(ECANCELED);

      return (size);
    }

    int			Interface::truncate(const char*		path,
					    off_t		size)
    {
      etoile::core::Object	file;
      Address			address;

      printf("[XXX] %s(%s, %u)\n",
	     __FUNCTION__,
	     path, size);

      // XXX check if it exists, check the perms etc.

      // resolve the path.
      if (Path::Resolve(path, address) == StatusError)
	issue(ENOENT);

      // retrieve the file.
      if (Hole::Get(address, file) == StatusError)
	issue(ENOENT);

      // truncate the file.
      if (File::Adjust(file, size, Interface::User.k) == StatusError)
	issue(ECANCELED);

      // store the updated file.
      if (File::Store(file) == StatusError)
	issue(ECANCELED);

      return (0);
    }

    int			Interface::release(const char*		path,
					   struct fuse_file_info* info)
    {
      printf("[XXX] %s(%s, 0x%x)\n",
	     __FUNCTION__,
	     path, info);

      /*
       * stub
       */

      return (0);
    }

    int			Interface::unlink(const char*		path)
    {
      etoile::core::Object	parent;
      etoile::core::Object	child;
      Address			address;
      String			directory;
      String			name;
      Natural32			size;

      printf("[XXX] %s(%s)\n",
	     __FUNCTION__,
	     path);

      // XXX check the perms etc.

      // resolve the directory path.
      if (Path::Resolve(path, address) == StatusError)
	issue(ENOENT);

      // load the target directory.
      if (Hole::Get(address, child) == StatusError)
	issue(ENOENT);

      // destroy the target file.
      if (File::Destroy(child) == StatusError)
	issue(ECANCELED);

      // retrieve the base and dir names.
      if (Path::Break(path, "/", directory, name) == StatusError)
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

    int			Interface::rename(const char*		source,
					  const char*		target)
    {
      etoile::core::Object	o_source;
      etoile::core::Object	o_target;
      etoile::core::Object	o_object;
      Address			a_source;
      Address			a_target;
      Address			a_object;
      String			d_source;
      String			d_target;
      String			n_source;
      String			n_target;

      printf("[XXX] %s(%s, %s)\n",
	     __FUNCTION__,
	     source, target);

      // XXX check the perms etc.
      // XXX[note this function does not remove the past blocks but we
      //     dont care]

      // retrieve the base and dir names.
      if (Path::Break(source, "/", d_source, n_source) == StatusError)
	issue(ECANCELED);

      // resolve the source path.
      if (Path::Resolve(d_source, a_source) == StatusError)
	issue(ENOENT);

      // load the source directory.
      if (Hole::Get(a_source, o_source) == StatusError)
	issue(ENOENT);

      // retrieve the base and dir names.
      if (Path::Break(target, "/", d_target, n_target) == StatusError)
	issue(ECANCELED);

      // resolve the target path.
      if (Path::Resolve(d_target, a_target) == StatusError)
	issue(ENOENT);

      // load the target directory.
      if (Hole::Get(a_target, o_target) == StatusError)
	issue(ENOENT);

      // resolve the object path.
      if (Path::Resolve(source, a_object) == StatusError)
	issue(ENOENT);

      // load the object.
      if (Hole::Get(a_object, o_object) == StatusError)
	issue(ENOENT);

      // if directories are different.
      if (a_source != a_target)
	{
	  // remove the entry source the souce directory.
	  if (Directory::Remove(o_source, n_source,
				Interface::User.k) == StatusError)
	    issue(ECANCELED);

	  // add an entry in the target directory.
	  if (Directory::Add(o_target, n_target, a_object,
			     Interface::User.k) == StatusError)
	    issue(ECANCELED);

	  // store the source directory.
	  if (Directory::Store(o_source) == StatusError)
	    issue(ECANCELED);

	  // store the target directory.
	  if (Directory::Store(o_target) == StatusError)
	    issue(ECANCELED);
	}
      else
	{
	  // the source and target directories are the same, just update
	  // one of the two.

	  // remove the entry source the souce directory.
	  if (Directory::Remove(o_source, n_source,
				Interface::User.k) == StatusError)
	    issue(ECANCELED);

	  // add an entry in the source directory.
	  if (Directory::Add(o_source, n_target, a_object,
			     Interface::User.k) == StatusError)
	    issue(ECANCELED);

	  // store the source directory.
	  if (Directory::Store(o_source) == StatusError)
	    issue(ECANCELED);
	}

      return (0);
    }

    int			Interface::symlink(const char*		target,
					   const char*		source)
    {
      etoile::core::Object	link;
      etoile::core::Object	directory;
      String			base;
      String			name;
      Address			address;
      Address			self;

      printf("[XXX] %s(%s, %s)\n",
	     __FUNCTION__,
	     target, source);

      // retrieve the base and dir names.
      if (Path::Break(source, "/", base, name) == StatusError)
	issue(ECANCELED);

      // resolve the parent directory path.
      if (Path::Resolve(base, address) == StatusError)
	issue(ENOENT);

      // retrieve the parent directory.
      if (Hole::Get(address, directory) == StatusError)
	issue(ENOENT);

      // create the link
      if (Link::Create(link, Interface::User) == StatusError)
	issue(ECANCELED);

      // set the relation.
      if (Link::Set(link, target, Interface::User.k) == StatusError)
	issue(ECANCELED);

      // store the object.
      if (Link::Store(link) == StatusError)
	issue(ECANCELED);

      // retrieve the address.
      if (link.Self(self) == StatusError)
	issue(ECANCELED);

      // add an entry in the parent directory.
      if (Directory::Add(directory, name, self,
			 Interface::User.k) == StatusError)
	issue(ECANCELED);

      // store the directory.
      if (Directory::Store(directory) == StatusError)
	issue(ECANCELED);

      return (0);
    }

    int			Interface::readlink(const char*		path,
					    char*		buffer,
					    size_t		size)
    {
      etoile::core::Object	link;
      Address			address;
      String			directory;
      String			name;
      String			target;

      printf("[XXX] %s(%s, 0x%x, %u)\n",
	     __FUNCTION__,
	     path, buffer, size);

      // XXX check the perms etc.

      // resolve the directory path.
      if (Path::Resolve(path, address) == StatusError)
	issue(ENOENT);

      // load the target directory.
      if (Hole::Get(address, link) == StatusError)
	issue(ENOENT);

      // resolve the link
      if (Link::Get(link, target) == StatusError)
	issue(ECANCELED);

      // copy the target in the given buffer.
      strncpy(buffer,
	      target.c_str(),
	      (size < target.length()) ? size : target.length());

      return (0);
    }

    int			Interface::mkdir(const char*		path,
					 mode_t			mode)
    {
      etoile::core::Object	parent;
      etoile::core::Object	child;
      Address			address;
      String			directory;
      String			name;
      Address			self;

      printf("[XXX] %s(%s, 0x%x)\n",
	     __FUNCTION__,
	     path, mode);

      // XXX check if it already exists, check the perms etc.

      // retrieve the base and dir names.
      if (Path::Break(path, "/", directory, name) == StatusError)
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

      // retrieve the address.
      if (child.Self(self) == StatusError)
	issue(ECANCELED);

      // add an entry in the parent directory.
      if (Directory::Add(parent, name, self,
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
      etoile::core::Object		directory;
      etoile::core::Catalog		catalog;
      etoile::core::Catalog::Iterator	iterator;
      Address				address;

      printf("[XXX] %s(%s, 0x%x, 0x%x, %u, 0x%x)\n",
	     __FUNCTION__,
	     path, buffer, filler, offset, info);

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
      if (directory.data.contents.type == Contents::TypeNone)
	return (0);

      // load the catalog.
      if (Hole::Get(directory.data.contents.address, catalog) == StatusError)
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
      etoile::core::Object	parent;
      etoile::core::Object	child;
      Address			address;
      String			directory;
      String			name;
      Natural32			size;

      printf("[XXX] %s(%s)\n",
	     __FUNCTION__,
	     path);

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

      // destroy the directory.
      if (Directory::Destroy(child) == StatusError)
	issue(ECANCELED);

      // retrieve the base and dir names.
      if (Path::Break(path, "/", directory, name) == StatusError)
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
