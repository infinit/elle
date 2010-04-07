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
// updated       julien quintard   [wed apr  7 01:04:30 2010]
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
  /// this defines the number of directories entries fetched from Etoile.
  ///
  const etoile::kernel::Size	PIG::Frame = 32;

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
    etoile::context::Identifier		identifier;
    etoile::wall::State			state;
    etoile::path::Way			way(path);

    printf("[XXX] %s(%s, 0x%x)\n",
	   __FUNCTION__,
	   path, stat);

    // clear the stat structure.
    ::memset(stat, 0x0, sizeof(struct stat));

    // load the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectLoad>(way),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // retrieve information on the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectInformation>(identifier),
	  Outputs<TagObjectState>(state)) == StatusError)
      error(ENOENT);

    // set the uid/gid
    /// XXX \todo use a mapping configuration file.
    stat->st_uid = getuid();
    stat->st_gid = getgid();

    // set the size.
    stat->st_size = (off_t)state.size;

    // convert the times into time_t structures.
    stat->st_atime = time(NULL);

    if (state.stamps.creation.Convert(stat->st_ctime) == StatusError)
      error(ENOENT);

    if (state.stamps.modification.Convert(stat->st_mtime) == StatusError)
      error(ENOENT);

    // set the mode and permissions.
    /// \todo XXX set properly the permissions according to the user's
    ///       access permissions + the EXEC bit with the attributes.
    switch (state.genre)
      {
      case etoile::kernel::GenreDirectory:
	{
	  stat->st_mode = S_IFDIR | 0700;

	  break;
	}
      case etoile::kernel::GenreFile:
	{
	  stat->st_mode = S_IFREG | 0700;

	  break;
	}
      case etoile::kernel::GenreLink:
	{
	  stat->st_mode = S_IFLNK | 0700;

	  break;
	}
      default:
	{
	  error(ENOENT);
	}
      }

    // store the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectStore>(identifier),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    return (0);
  }

  ///
  /// this method opens the directory _path_.
  ///
  int			PIG::Opendir(const char*		path,
				     struct fuse_file_info*	info)
  {
    etoile::context::Identifier		identifier;
    etoile::path::Way			way(path);
    etoile::kernel::Subject		subject;
    etoile::kernel::Record		record;

    printf("[XXX] %s(%s, 0x%x)\n",
	   __FUNCTION__,
	   path, info);

    // load the object.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryLoad>(way),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // create a subject.
    if (subject.Create(PIG::K) == StatusError)
      error(ENOENT);

    // retrieve the user's permissions on the object.
    if (PIG::Channel.Call(
	  Inputs<TagAccessLookup>(identifier, subject),
	  Outputs<TagAccessRecord>(record)) == StatusError)
      error(ENOENT);

    // check if the user has the right to read the directory.
    if ((record.permissions & etoile::kernel::PermissionRead) == 0)
      error(ENOENT);

    // duplicate the identifier and save it in the info structure's file
    // handle.
    info->fh = (uint64_t)new etoile::context::Identifier(identifier);

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
    etoile::path::Way			way(path);
    etoile::context::Identifier*	identifier;
    etoile::wall::State			state;

    printf("[XXX] %s(%s, 0x%x, 0x%x, %u, 0x%x)\n",
	   __FUNCTION__,
	   path, buffer, filler, offset, info);

    // set the identifier pointer to the file handle that has been
    // filled by Opendir().
    identifier = (etoile::context::Identifier*)info->fh;

    /// XXX \todo these . and .. entries are UNIX-specific
    // fill the . and .. entries.
    if (offset == 0)
      filler(buffer, ".", NULL, 1);
    if (offset <= 1)
      filler(buffer, "..", NULL, 2);

    while (true)
      {
	etoile::kernel::Set::Scoutor	scoutor;
	etoile::kernel::Set		set;

	// read the directory entries.
	if (PIG::Channel.Call(
	      Inputs<TagDirectoryConsult>(*identifier,
					  offset - 2,
					  PIG::Frame),
	      Outputs<TagDirectorySet>(set)) == StatusError)
	  error(ENOENT);

	// add the entries by using the filler() function.
	for (scoutor = set.container.begin();
	     scoutor != set.container.end();
	     scoutor++)
	  {
	    ::etoile::kernel::Entry*	entry = *scoutor;

	    // fill the buffer with filler().
	    if (filler(buffer, entry->name.c_str(), NULL, offset++) == 1)
	      return (0);
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
    etoile::context::Identifier*	identifier;

    printf("[XXX] %s(%s, 0x%x)\n",
	   __FUNCTION__,
	   path, info);

    // set the identifier pointer to the file handle that has been
    // filled by Opendir().
    identifier = (etoile::context::Identifier*)info->fh;

    // store the context back.
    // store the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectStore>(*identifier),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    return (0);
  }

  ///
  /// this method creates a directory.
  ///
  int			PIG::Mkdir(const char*			path,
				   mode_t			mode)
  {
    etoile::path::Slice	name;
    etoile::path::Way	way(path, name);

    printf("HERE\n");
    way.Dump();
    std::cout << name << std::endl;
    printf("/HERE\n");

    return (0);
  }

  ///
  /// this method removes a directory.
  int			PIG::Rmdir(const char*			path)
  {

    return (0);
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
      operations.getattr = PIG::Getattr;

      operations.opendir = PIG::Opendir;
      operations.readdir = PIG::Readdir;
      operations.releasedir = PIG::Releasedir;
      operations.mkdir = PIG::Mkdir;
      operations.rmdir = PIG::Rmdir;

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
