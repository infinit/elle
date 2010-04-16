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
// updated       julien quintard   [fri apr 16 16:28:07 2010]
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
  /// this structure contains the function pointers to the FUSE implementation
  /// routines.
  ///
  static struct fuse_operations	operations;

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string contains the path to the user infinit configuration
  /// directory.
  ///
  const String			PIG::Path = System::Path::Home + "/.infinit";

  ///
  /// this string represents the door's name to Etoile.
  ///
  const String			PIG::Line("etoile");

  ///
  /// the door to Etoile.
  ///

  Door				PIG::Channel;

  ///
  /// the public key of the user the application is running on behalf of.
  ///
  PublicKey			PIG::K;

  ///
  /// the phrase used to connect applications to Etoile.
  ///
  String			PIG::Phrase;

  ///
  /// the subject representing the current user.
  ///
  ::etoile::kernel::Subject	PIG::Subject;

  ///
  /// this defines the number of directories entries fetched from Etoile.
  ///
  const ::etoile::kernel::Size	PIG::Frame = 32;

//
// ---------- callbacks -------------------------------------------------------
//

  ///
  /// this method returns general-purpose information on the file system
  /// object identified by _path_.
  ///
  int			PIG::Getattr(const char*		path,
				     struct stat*		stat)
  {
    ::etoile::context::Identifier	identifier;
    ::etoile::wall::State		state;
    ::etoile::path::Way			way(path);
    ::etoile::kernel::Record		record;

    printf("[XXX] %s(%s, 0x%x)\n",
	   __FUNCTION__,
	   path, stat);

    // clear the stat structure.
    ::memset(stat, 0x0, sizeof(struct stat));

    // XXX un moyen de tester si l'objet exist?

    // load the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectLoad>(way),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // retrieve information on the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectInformation>(identifier),
	  Outputs<TagObjectState>(state)) == StatusError)
      error(EINTR);

    // XXX ici ne pas chopper les perms du user mais les perms du proprio!
    // donc utiliser le mapping pour recuperer sa clef etc.

    // retrieve the user's permissions on the object.
    if (PIG::Channel.Call(
	  Inputs<TagAccessLookup>(identifier, PIG::Subject),
	  Outputs<TagAccessRecord>(record)) == StatusError)
      error(EINTR);

    // check the record.
    if (record == kernel::Record::Null)
      error(EACCES);

    // set the uid/gid
    /// XXX \todo use a mapping configuration file.
    /// XXX mapping pour user et group getgid() et ne pas mettre de perms.
    stat->st_uid = getuid();
    stat->st_gid = getgid();

    // set the size.
    stat->st_size = (off_t)state.size;

    // convert the times into time_t structures.
    stat->st_atime = time(NULL);

    if (state.stamps.creation.Convert(stat->st_ctime) == StatusError)
      error(EINTR);

    if (state.stamps.modification.Convert(stat->st_mtime) == StatusError)
      error(EINTR);

    // set the mode and permissions.
    switch (state.genre)
      {
      case ::etoile::kernel::GenreDirectory:
	{
	  // set the object as being a directory.
	  stat->st_mode = S_IFDIR;

	  // if the user has the read permission, allow her to access
	  // and read the directory.
	  if ((record.permissions & ::etoile::kernel::PermissionRead) != 0)
	    stat->st_mode |= S_IRUSR | S_IXUSR;

	  // if the user has the write permission, allow her to modify
	  // the directory content.
	  if ((record.permissions & ::etoile::kernel::PermissionWrite) != 0)
	    stat->st_mode |= S_IWUSR;

	  break;
	}
      case ::etoile::kernel::GenreFile:
	{
	  ::etoile::kernel::Trait	trait;

	  stat->st_mode = S_IFREG;

	  // if the user has the read permission, allow her to read
	  // the file.
	  if ((record.permissions & ::etoile::kernel::PermissionRead) != 0)
	    stat->st_mode |= S_IRUSR;

	  // if the user has the write permission, allow her to modify
	  // the file content.
	  if ((record.permissions & ::etoile::kernel::PermissionWrite) != 0)
	    stat->st_mode |= S_IWUSR;

	  /*
	  // retrieve the attribute.
	  if (PIG::Channel.Call(
	        Inputs<TagAttributesLookup>(identifier, "posix::exec"),
		Outputs<TagAttributesTrait>(trait)) == StatusError)
	    error(ENOENT);

	  // check the trait.
	  if ((trait != ::etoile::kernel::Trait::Null) &&
	      (trait->value == "true"))
	    {
	      // active the exec bit.
	      stat->st_mode |= S_IXUSR;
	    }
	  */

	  break;
	}
      case ::etoile::kernel::GenreLink:
	{
	  stat->st_mode = S_IFLNK;

	  // if the user has the read permission, allow her to read
	  // the file.
	  if ((record.permissions & ::etoile::kernel::PermissionRead) != 0)
	    stat->st_mode |= S_IRUSR;

	  // if the user has the write permission, allow her to modify
	  // the file content.
	  if ((record.permissions & ::etoile::kernel::PermissionWrite) != 0)
	    stat->st_mode |= S_IWUSR;

	  /*
	  // retrieve the attribute.
	  if (PIG::Channel.Call(
	        Inputs<TagAttributesLookup>(identifier, "posix::exec"),
		Outputs<TagAttributesTrait>(trait)) == StatusError)
	    error(ENOENT);

	  // check the trait.
	  if ((trait != ::etoile::kernel::Trait::Null) &&
	      (trait->value == "true"))
	    {
	      // active the exec bit.
	      stat->st_mode |= S_IXUSR;
	    }
	  */

	  break;
	}
      default:
	{
	  error(EINTR);
	}
      }

    // store the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectStore>(identifier),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    return (0);
  }

  ///
  /// this method opens the directory _path_.
  ///
  int			PIG::Opendir(const char*		path,
				     struct fuse_file_info*	info)
  {
    ::etoile::context::Identifier	identifier;
    ::etoile::path::Way			way(path);
    ::etoile::kernel::Record		record;

    printf("[XXX] %s(%s, 0x%x)\n",
	   __FUNCTION__,
	   path, info);

    // load the object.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryLoad>(way),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // XXX est-ce necessaire de checker, si ca se trouve c'est deja fait
    // par FUSE

    // retrieve the user's permissions on the object.
    if (PIG::Channel.Call(
	  Inputs<TagAccessLookup>(identifier, PIG::Subject),
	  Outputs<TagAccessRecord>(record)) == StatusError)
      error(EINTR);

    // check the record.
    if (record == kernel::Record::Null)
      error(EACCES);

    // check if the user has the right to read the directory.
    if ((record.permissions & ::etoile::kernel::PermissionRead) == 0)
      error(EACCES);

    // duplicate the identifier and save it in the info structure's file
    // handle.
    info->fh = (uint64_t)new ::etoile::context::Identifier(identifier);

    return (0);
  }

  ///
  /// this method reads the directory entries.
  ///
  int			PIG::Readdir(const char*		path,
				     void*			buffer,
				     fuse_fill_dir_t		filler,
				     off_t			offset,
				     struct fuse_file_info*	info)
  {
    ::etoile::path::Way			way(path);
    ::etoile::context::Identifier*	identifier;
    ::etoile::wall::State		state;
    off_t				next;

    printf("[XXX] %s(%s, 0x%x, 0x%x, %u, 0x%x)\n",
	   __FUNCTION__,
	   path, buffer, filler, offset, info);

    // set the identifier pointer to the file handle that has been
    // filled by Opendir().
    identifier = (::etoile::context::Identifier*)info->fh;

    /// XXX \todo these . and .. entries are UNIX-specific
    // fill the . and .. entries.
    if (offset == 0)
      filler(buffer, ".", NULL, 1);
    if (offset <= 1)
      filler(buffer, "..", NULL, 2);

    // adjust the offset since Etoile starts with zero while in POSIX
    // terms, zero and one are used for . and ..
    if (offset > 2)
      offset -= 2;

    // compute the offset of the next entry.
    if (offset < 2)
      next = 3;
    else
      next = offset + 1;

    while (true)
      {
	::etoile::kernel::Set::Scoutor	scoutor;
	::etoile::kernel::Set		set;

	// read the directory entries.
	if (PIG::Channel.Call(
	      Inputs<TagDirectoryConsult>(*identifier,
					  offset,
					  PIG::Frame),
	      Outputs<TagDirectorySet>(set)) == StatusError)
	  error(EINTR);

	// add the entries by using the filler() function.
	for (scoutor = set.container.begin();
	     scoutor != set.container.end();
	     scoutor++)
	  {
	    ::etoile::kernel::Entry*	entry = *scoutor;

	    printf("[XXX] filling %s (%qd)\n",
		   entry->name.c_str(),
		   next);

	    // fill the buffer with filler().
	    if (filler(buffer, entry->name.c_str(), NULL, next) == 1)
	      return (0);

	    // compute the offset of the next entry.
	    next++;
	  }

	if (set.container.size() < PIG::Frame)
	  break;
      }

    return (0);
  }

  ///
  /// this method closes the directory _path_.
  ///
  int			PIG::Releasedir(const char*		path,
					struct fuse_file_info*	info)
  {
    ::etoile::context::Identifier*	identifier;

    printf("[XXX] %s(%s, 0x%x)\n",
	   __FUNCTION__,
	   path, info);

    // set the identifier pointer to the file handle that has been
    // filled by Opendir().
    identifier = (::etoile::context::Identifier*)info->fh;

    // store the context back.
    // store the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectStore>(*identifier),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    // delete the identifier.
    delete identifier;

    // reset the file handle, just to make sure it is not used anymore.
    info->fh = 0;

    return (0);
  }

  ///
  /// this method creates a directory.
  ///
  int			PIG::Mkdir(const char*			path,
				   mode_t			mode)
  {
    ::etoile::path::Slice		name;
    ::etoile::path::Way			way(::etoile::path::Way(path), name);
    ::etoile::context::Identifier	directory;
    ::etoile::context::Identifier	subdirectory;

    printf("[XXX] %s(%s, 0%o)\n",
	   __FUNCTION__,
	   path, mode);

    // XXX check perms?

    // load the directory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryLoad>(way),
	  Outputs<TagIdentifier>(directory)) == StatusError)
      error(ENOENT);

    // create the subdirectory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryCreate>(),
	  Outputs<TagIdentifier>(subdirectory)) == StatusError)
      error(EINTR);

    // XXX set permissions

    // add the subdirectory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryAdd>(directory, name, subdirectory),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    // store the subdirectory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryStore>(subdirectory),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    // store the directory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryStore>(directory),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    return (0);
  }

  ///
  /// this method removes a directory.
  ///
  int			PIG::Rmdir(const char*			path)
  {
    ::etoile::path::Slice		name;
    ::etoile::path::Way			child(path);
    ::etoile::path::Way			parent(child, name);
    ::etoile::context::Identifier	directory;
    ::etoile::context::Identifier	subdirectory;

    printf("[XXX] %s(%s)\n",
	   __FUNCTION__,
	   path);

    // check perms?

    // load the directory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryLoad>(parent),
	  Outputs<TagIdentifier>(directory)) == StatusError)
      error(ENOENT);

    // load the subdirectory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryLoad>(child),
	  Outputs<TagIdentifier>(subdirectory)) == StatusError)
      error(ENOENT);

    // remove the entry.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryRemove>(directory, name),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    // store the directory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryStore>(directory),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    // destroy the subdirectory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryDestroy>(subdirectory),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    return (0);
  }

  ///
  /// this method checks if the current user has the permission to access
  /// the object _path_ for the operations _mask_.
  ///
  int			PIG::Access(const char*			path,
				    int				mask)
  {
    ::etoile::context::Identifier	identifier;
    ::etoile::wall::State		state;
    ::etoile::path::Way			way(path);
    ::etoile::kernel::Record		record;

    printf("[XXX] %s(%s, 0%o)\n",
	   __FUNCTION__,
	   path, mask);

    // load the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectLoad>(way),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // retrieve information on the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectInformation>(identifier),
	  Outputs<TagObjectState>(state)) == StatusError)
      error(EINTR);

    // retrieve the user's permissions on the object.
    if (PIG::Channel.Call(
	  Inputs<TagAccessLookup>(identifier, PIG::Subject),
	  Outputs<TagAccessRecord>(record)) == StatusError)
      error(ENOENT);

    // check the record.
    if (record == kernel::Record::Null)
      error(EACCES);

    // check if the permissions match the mask for execution.
    if ((mask & X_OK) != 0)
      {
	if ((state.genre == ::etoile::kernel::GenreDirectory) &&
	    ((record.permissions & ::etoile::kernel::PermissionRead) == 0))
	  error(EACCES);

	// XXX /* attribute X pas present */ retrieve attribute!
      }

    // check if the permissions match the mask for reading.
    if ((mask & R_OK) != 0)
      {
	if ((record.permissions & ::etoile::kernel::PermissionRead) == 0)
	  error(EACCES);
      }

    // check if the permissions match the mask for writing.
    if ((mask & W_OK) != 0)
      {
	if ((record.permissions & ::etoile::kernel::PermissionWrite) == 0)
	  error(EACCES);
      }

    return (0);
  }

  /*
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

  */

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
    {
      if (Elle::Initialize() == StatusError)
	escape("unable to initialize the Elle library");
    }

    //
    // read the phrase from the file.
    //
    {
      String		path = PIG::Path + "/phrase";
      Region		region;
      Archive		archive;
      struct ::stat	stat;
      Integer32		fd;

      // get the file status.
      if (::stat(path.c_str(), &stat) == -1)
	escape("unable to access the phrase file");

      // prepare the region.
      if (region.Prepare(stat.st_size) == StatusError)
	escape("unable to prepare the region");

      // set the region size.
      region.size = stat.st_size;

      // open the file.
      if ((fd = ::open(path.c_str(), O_RDONLY)) == -1)
	escape(::strerror(errno));

      // read the file.
      if (::read(fd, region.contents, region.size) != region.size)
	{
	  ::close(fd);

	  escape("unable to read the phrase file");
	}

      // close the file.
      ::close(fd);

      // detach the region.
      if (region.Detach() == StatusError)
	escape("unable to detach the region");

      // prepare the archive.
      if (archive.Prepare(region) == StatusError)
	escape("unable to detach the region");

      // extract the phrase.
      if (archive.Extract(PIG::K, PIG::Phrase) == StatusError)
	escape("unable to extract the phrase");
    }

    //
    // create a subject representing the current user.
    //
    {
      // create the subject.
      if (PIG::Subject.Create(PIG::K) == StatusError)
	escape("unable to create the user's subject");
    }

    //
    // connect the application to etoile.
    //
    {
      // create the door.
      if (PIG::Channel.Create(Socket::ModeSynchronous) == StatusError)
	escape("unable to create the door");

      // connect the door.
      if (PIG::Channel.Connect(PIG::Line) == StatusError)
	escape("unable to connect to Etoile");

      // connect to etoile by providing the phrase which links the
      // application to the agent.
      if (PIG::Channel.Call(
	    Inputs< ::etoile::TagWallConnect >(PIG::Phrase),
	    Outputs< ::etoile::TagOk >()) == StatusError)
	escape("unable to connect to etoile");
    }

    //
    // initialize the fuse operations.
    //
    {
      // set the operations.

      // operations.statfs: not supported

      operations.getattr = PIG::Getattr;

      operations.opendir = PIG::Opendir;
      operations.readdir = PIG::Readdir;
      operations.releasedir = PIG::Releasedir;
      operations.mkdir = PIG::Mkdir;
      operations.rmdir = PIG::Rmdir;

      operations.access = PIG::Access;
      /*
      operations.chmod = PIG::Chmod;
      operations.chown = PIG::Chown;
      operations.setxattr = PIG::Setxattr;
      operations.getxattr = PIG::Getxattr;
      operations.listxattr = PIG::Listxattr;
      operations.removexattr = PIG::Removexattr;
      */
      /*

	operations.utimens = PIG::utimens; // not permitted

	operations.mknod = PIG::mknod; // use create() instead

	// file
	operations.open = PIG::open;
	operations.read = PIG::read;
	operations.write = PIG::write;
	operations.truncate = PIG::truncate;
	operations.release = PIG::release;
	operations.unlink = PIG::unlink;

	// link
	operations.readlink = PIG::readlink;
	operations.symlink = PIG::symlink;

	// objects
	operations.rename = PIG::rename;
      */
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
// ---------- functions -------------------------------------------------------
//

  ///
  /// this is the entry point of pig.
  ///
  Status		Main(const Natural32			argc,
			     const Character*			argv[])
  {
    enter();

    // initialize the module.
    if (PIG::Initialize() == StatusError)
      escape("unable to initialize the module");

    // launch fuse.
    if (fuse_main((int)argc, (char**)argv, &operations, NULL) != 0)
      escape("an error occured in FUSE");

    // clean the module.
    if (PIG::Clean() == StatusError)
      escape("unable to clean the module");

    leave();
  }

}

//
// ---------- main ------------------------------------------------------------
//

int			main(const int			argc,
			     const char*		argv[])
{
  pig::Main(argc, argv);

  expose();

  return (0);
}
