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
// updated       julien quintard   [tue apr 20 21:47:33 2010]
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
  const String				PIG::Path =
    System::Path::Home + "/.infinit";

  ///
  /// this string represents the door's name to Etoile.
  ///
  const String				PIG::Line("etoile");

  ///
  /// the door to Etoile.
  ///

  Door					PIG::Channel;

  ///
  /// the public key of the user the application is running on behalf of.
  ///
  PublicKey				PIG::K;

  ///
  /// the phrase used to connect applications to Etoile.
  ///
  String				PIG::Phrase;

  ///
  /// the subject representing the current user.
  ///
  etoile::kernel::Subject		PIG::Subject;

  ///
  /// this defines the number of directories entries fetched from Etoile.
  ///
  const etoile::kernel::Size		PIG::Frame = 32;

  ///
  /// this variable contains the UID of the 'somebody' user, user which
  /// is used whenever the system cannot map the Infinit user on a local
  /// user.
  ///
  uid_t					PIG::Somebody::UID;

  ///
  /// this variable contains the GID of the 'somebody' group.
  ///
  gid_t					PIG::Somebody::GID;

  ///
  /// this varaible contains the mappings between local user identities and
  /// Infinit identities.
  ///
  map::Map<PublicKey>			PIG::Maps::Users;

  ///
  /// this varaible contains the mappings between local group identities and
  /// Infinit identities.
  ///
  map::Map<etoile::hole::Address>	PIG::Maps::Groups;

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
    etoile::context::Identifier	identifier;
    etoile::path::Way		way(path);
    struct fuse_file_info	info;
    int				result;

    printf("[XXX] %s(%s, 0x%x)\n",
	   __FUNCTION__,
	   path, stat);

    // XXX un moyen de tester si l'objet exist?
    // XXX faire un module de wall: Path

    // load the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectLoad>(way),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // set the identifier in the fuse_file_info structure.
    info.fh = (uint64_t)&identifier;

    // call the Fgetattr() method.
    result = PIG::Fgetattr(path, stat, &info);

    // store the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectStore>(identifier),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    return (result);
  }

  ///
  /// this method returns general-purpose information on the file system
  /// object identified by _path_.
  ///
  int			PIG::Fgetattr(const char*		path,
				      struct stat*		stat,
				      struct fuse_file_info*	info)
  {
    etoile::context::Identifier*	identifier;
    etoile::wall::State			state;
    etoile::path::Way			way(path);
    const String*			name;

    printf("[XXX] %s(%s, 0x%x)\n",
	   __FUNCTION__,
	   path, stat);

    // clear the stat structure.
    ::memset(stat, 0x0, sizeof(struct stat));

    // retrieve the identifier.
    identifier = (etoile::context::Identifier*)info->fh;

    // retrieve information on the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectInformation>(*identifier),
	  Outputs<TagObjectState>(state)) == StatusError)
      error(EINTR);

    // set the uid by first looking into the users map. if no local user is
    // found, the 'somebody' user is used instead, indicating that the
    // file belongs to someone, with the given permissions, but cannot
    // be mapped to a local user name.
    if (PIG::Maps::Users.Lookup(state.keys.owner, name) == StatusTrue)
      {
	//
	// in this case, the object's owner is known locally.
	//
	struct ::passwd*	passwd;

	// retrieve the passwd structure associated with this name.
	if ((passwd = ::getpwnam(name->c_str())) != NULL)
	  {
	    // set the uid to the local user's.
	    stat->st_uid = passwd->pw_uid;
	  }
	else
	  {
	    // if an error occured, set the user to 'somebody'.
	    stat->st_uid = PIG::Somebody::UID;
	  }
      }
    else
      {
	//
	// otherwise, this user is unknown locally, so indicate the
	// system that this object belongs to the 'somebody' user.
	//

	stat->st_uid = PIG::Somebody::UID;
      }

    // since Infinit does not have the concept of current group, the
    // group of this object is set to 'somebody'.
    stat->st_gid = PIG::Somebody::GID;

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
      case etoile::kernel::GenreDirectory:
	{
	  // set the object as being a directory.
	  stat->st_mode = S_IFDIR;

	  // if the user has the read permission, allow her to access
	  // and read the directory.
	  if ((state.permissions.owner &
	       etoile::kernel::PermissionRead) != 0)
	    stat->st_mode |= S_IRUSR | S_IXUSR;

	  // if the user has the write permission, allow her to modify
	  // the directory content.
	  if ((state.permissions.owner &
	       etoile::kernel::PermissionWrite) != 0)
	    stat->st_mode |= S_IWUSR;

	  break;
	}
      case etoile::kernel::GenreFile:
	{
	  etoile::kernel::Trait	trait;

	  stat->st_mode = S_IFREG;

	  // if the user has the read permission, allow her to read
	  // the file.
	  if ((state.permissions.owner &
	       etoile::kernel::PermissionRead) != 0)
	    stat->st_mode |= S_IRUSR;

	  // if the user has the write permission, allow her to modify
	  // the file content.
	  if ((state.permissions.owner &
	       etoile::kernel::PermissionWrite) != 0)
	    stat->st_mode |= S_IWUSR;

	  /*
	  // retrieve the attribute.
	  if (PIG::Channel.Call(
	        Inputs<TagAttributesLookup>(identifier, "posix::exec"),
		Outputs<TagAttributesTrait>(trait)) == StatusError)
	    error(ENOENT);

	  // check the trait.
	  if ((trait != etoile::kernel::Trait::Null) &&
	      (trait->value == "true"))
	    {
	      // active the exec bit.
	      stat->st_mode |= S_IXUSR;
	    }
	  */

	  break;
	}
      case etoile::kernel::GenreLink:
	{
	  stat->st_mode = S_IFLNK;

	  // if the user has the read permission, allow her to read and
	  // search the linked object.
	  if ((state.permissions.owner &
	       etoile::kernel::PermissionRead) != 0)
	    stat->st_mode |= S_IRUSR | S_IXUSR;

	  // if the user has the write permission, allow her to modify
	  // the link.
	  if ((state.permissions.owner &
	       etoile::kernel::PermissionWrite) != 0)
	    stat->st_mode |= S_IWUSR;

	  break;
	}
      default:
	{
	  error(EINTR);
	}
      }

    return (0);
  }

  ///
  /// this method changes the access and modification time of the object.
  ///
  int			PIG::Utimens(const char*		path,
				     const struct timespec	tv[2])
  {
    printf("[XXX] %s(%s, ...)\n",
	   __FUNCTION__,
	   path);

    //
    // not supported
    //

    return (0);
  }

  ///
  /// this method opens the directory _path_.
  ///
  int			PIG::Opendir(const char*		path,
				     struct fuse_file_info*	info)
  {
    etoile::context::Identifier	identifier;
    etoile::path::Way		way(path);
    etoile::kernel::Record	record;

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
    if (record == etoile::kernel::Record::Null)
      error(EACCES);

    // check if the user has the right to read the directory.
    if ((record.permissions & etoile::kernel::PermissionRead) == 0)
      error(EACCES);

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
    etoile::path::Way		way(path);
    etoile::context::Identifier*identifier;
    etoile::wall::State		state;
    off_t			next;

    printf("[XXX] %s(%s, 0x%x, 0x%x, %qu, 0x%x)\n",
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
	etoile::kernel::Range<etoile::kernel::Entry>		range;
	etoile::kernel::Range<etoile::kernel::Entry>::Scoutor	scoutor;

	// read the directory entries.
	if (PIG::Channel.Call(
	      Inputs<TagDirectoryConsult>(*identifier,
					  etoile::kernel::Offset(offset),
					  PIG::Frame),
	      Outputs<TagDirectoryRange>(range)) == StatusError)
	  error(EINTR);

	// add the entries by using the filler() function.
	for (scoutor = range.container.begin();
	     scoutor != range.container.end();
	     scoutor++)
	  {
	    etoile::kernel::Entry*	entry = *scoutor;

	    // fill the buffer with filler().
	    if (filler(buffer, entry->name.c_str(), NULL, next) == 1)
	      return (0);

	    // compute the offset of the next entry.
	    next++;
	  }

	if (range.container.size() < PIG::Frame)
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
	  Inputs<TagDirectoryStore>(*identifier),
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
    etoile::path::Slice		name;
    etoile::path::Way		way(etoile::path::Way(path), name);
    etoile::context::Identifier	directory;
    etoile::context::Identifier	subdirectory;
    etoile::kernel::Permissions	permissions;

    printf("[XXX] %s(%s, 0%o)\n",
	   __FUNCTION__,
	   path, mode);

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

    // compute the permissions.
    if ((mode & S_IRUSR) != 0)
      permissions |= etoile::kernel::PermissionRead;

    if ((mode & S_IWUSR) != 0)
      permissions |= etoile::kernel::PermissionWrite;

    // set the owner permissions.
    if (PIG::Channel.Call(
	  Inputs<TagAccessUpdate>(subdirectory, PIG::Subject, permissions),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

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
    etoile::path::Slice		name;
    etoile::path::Way		child(path);
    etoile::path::Way		parent(child, name);
    etoile::context::Identifier	directory;
    etoile::context::Identifier	subdirectory;

    printf("[XXX] %s(%s)\n",
	   __FUNCTION__,
	   path);

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
      error(EACCES);

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
    etoile::context::Identifier	identifier;
    etoile::wall::State		state;
    etoile::path::Way		way(path);
    etoile::kernel::Record	record;

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

    // store the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectStore>(identifier),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    // check the record.
    if (record == etoile::kernel::Record::Null)
      error(EACCES);

    // check if the permissions match the mask for execution.
    if ((mask & X_OK) != 0)
      {
	if ((state.genre == etoile::kernel::GenreDirectory) &&
	    ((record.permissions & etoile::kernel::PermissionRead) == 0))
	  error(EACCES);

	// XXX /* attribute X pas present */ retrieve attribute!
      }

    // check if the permissions match the mask for reading.
    if ((mask & R_OK) != 0)
      {
	if ((record.permissions & etoile::kernel::PermissionRead) == 0)
	  error(EACCES);
      }

    // check if the permissions match the mask for writing.
    if ((mask & W_OK) != 0)
      {
	if ((record.permissions & etoile::kernel::PermissionWrite) == 0)
	  error(EACCES);
      }

    return (0);
  }

  ///
  /// this method modifies the permissions on the object.
  ///
  int			PIG::Chmod(const char*			path,
				   mode_t			mode)
  {
    etoile::context::Identifier	identifier;
    etoile::path::Way		way(path);
    etoile::kernel::Permissions	permissions;

    printf("[XXX] %s(%s, 0%o)\n",
	   __FUNCTION__,
	   path, mode);

    //
    // note that this method ignores both the group and other permissions.
    //
    // in order not to ignore them, the system would have to create/update
    // the group entries in the object's access list. although this is
    // completely feasible, it has been decided not to do so because it
    // would incur too much cost.
    //
    // indeed, on most UNIX systems, the umask is set to 022 or is somewhat
    // equivalent, granting permissions, by default, to the default group
    // and the others.
    //
    // although this is, from our point of view, a very bad idea, it would
    // be catastrophic to create such access entries in Infinit, especially
    // because Infinit has been designed and optimised for objects
    // accessed by their sole owners.
    //

    // XXX sticky-bit etc. in attributes!

    // compute the permissions.
    if ((mode & S_IRUSR) != 0)
      permissions |= etoile::kernel::PermissionRead;

    if ((mode & S_IWUSR) != 0)
      permissions |= etoile::kernel::PermissionWrite;

    // XXX EXEC

    // load the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectLoad>(way),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // update the accesses.
    //
    // note that the method assumes that the caller is the object's owner!
    // if not, an error will occur anyway, so why bother checking.
    if (PIG::Channel.Call(
	  Inputs<TagAccessUpdate>(identifier, PIG::Subject, permissions),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    // store the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectStore>(identifier),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    return (0);
  }

  ///
  /// this method modifies the owner of a given object.
  ///
  int			PIG::Chown(const char*			path,
				   uid_t			uid,
				   gid_t			gid)
  {
    printf("[XXX] %s(%s, %u, %u)\n",
	   __FUNCTION__,
	   path, uid, gid);

    //
    // this functionality is not implemented because, in Infinit, changing
    // the ownership is made impossible through the design.
    //
    // indeed, since every object is bound to its owner through the use
    // of asymmetric cryptosystems, a current owner changing to ownership
    // to another would still be able, in the future, to re-change the
    // ownership, since still owning the original private key.
    //
    // therefore, changing ownership would imply copying the object.
    //
    // XXX \todo c'est carrement possible cela dit puisque seul la coquille
    // ie l'objet devra etre duplique. access et contents ne changent pas
    // et on prend juste leur adresse donc ca se resume en new object, copy
    // object, delete old object et voila. seul probleme, le owner precedent
    // aura la clef des donnees et pourra donc toujours y acceder: faux
    // probleme! donc niquel.
    //

    // XXX
    error(EINTR);

    return (0);
  }

  ///
  /// this method sets an extended attribute value.
  ///
  /// note that the flags are ignored!
  ///
  int			PIG::Setxattr(const char*		path,
				      const char*		name,
				      const char*		value,
				      size_t			size,
				      int			flags)
  {
    etoile::context::Identifier	identifier;
    etoile::path::Way		way(path);

    printf("[XXX] %s(%s, %s, 0x%x, %u, 0x%x)\n",
	   __FUNCTION__,
	   path, name, value, size, flags);

    // load the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectLoad>(way),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // set the attribute.
    if (PIG::Channel.Call(
	  Inputs<TagAttributesSet>(identifier,
				   String(name),
				   String(value, size)),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    // store the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectStore>(identifier),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    return (0);
  }

  ///
  /// this method returns the attribute associated with the given object.
  ///
  int			PIG::Getxattr(const char*		path,
				      const char*		name,
				      char*			value,
				      size_t			size)
  {
    etoile::context::Identifier	identifier;
    etoile::path::Way		way(path);
    etoile::kernel::Trait	trait;

    printf("[XXX] %s(%s, %s, 0x%x, %u)\n",
	   __FUNCTION__,
	   path, name, value, size);

    // load the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectLoad>(way),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // get the attribute.
    if (PIG::Channel.Call(
	  Inputs<TagAttributesGet>(identifier,
				   String(name)),
	  Outputs<TagAttributesTrait>(trait)) == StatusError)
      error(ENOENT);

    // store the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectStore>(identifier),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    // test if a trait has been found.
    if (trait == etoile::kernel::Trait::Null)
      error(ENOATTR);

    // if the size is null, it means that this call must be considered
    // as a request for the size required to store the value.
    if (size == 0)
      {
	return (trait.value.length());
      }
    else
      {
	// otherwise, copy the trait value in the value buffer.
	::memcpy(value, trait.value.data(), trait.value.length());

	// return the length of the value.
	return (trait.value.length());
      }
  }

  ///
  /// this method returns the list of attribute names.
  ///
  int			PIG::Listxattr(const char*		path,
				       char*			list,
				       size_t			size)
  {
    etoile::context::Identifier					identifier;
    etoile::path::Way						way(path);
    etoile::kernel::Range<etoile::kernel::Trait>		range;
    etoile::kernel::Range<etoile::kernel::Trait>::Scoutor	scoutor;
    size_t							offset;

    printf("[XXX] %s(%s, 0x%x, %u)\n",
	   __FUNCTION__,
	   path, list, size);

    // load the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectLoad>(way),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // fetch the attributes.
    if (PIG::Channel.Call(
	  Inputs<TagAttributesFetch>(identifier),
	  Outputs<TagAttributesRange>(range)) == StatusError)
      error(ENOENT);

    // store the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectStore>(identifier),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    // if the size is zero, this call must return the size required to
    // store the list.
    if (size == 0)
      {
	for (scoutor = range.container.begin();
	     scoutor != range.container.end();
	     scoutor++)
	  {
	    etoile::kernel::Trait*	trait = *scoutor;

	    // compute the size.
	    size = size + trait->name.length() + 1;
	  }

	return (size);
      }
    else
      {
	// otherwise, go through the attributes and concatenate their names.
	for (scoutor = range.container.begin(), offset = 0;
	     scoutor != range.container.end();
	     scoutor++)
	  {
	    etoile::kernel::Trait*	trait = *scoutor;

	    // concatenate the name.
	    ::strcpy(list + offset,
		     trait->name.c_str());

	    // adjust the offset.
	    offset = offset + trait->name.length() + 1;
	  }

	return (offset);
      }
  }

  ///
  /// this method removes an attribute.
  ///
  int			PIG::Removexattr(const char*		path,
					 const char*		name)
  {
    etoile::context::Identifier	identifier;
    etoile::path::Way		way(path);

    printf("[XXX] %s(%s, %s)\n",
	   __FUNCTION__,
	   path, name);

    // load the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectLoad>(way),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // omit the attribute.
    if (PIG::Channel.Call(
	  Inputs<TagAttributesOmit>(identifier,
				    String(name)),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    // store the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectStore>(identifier),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    return (0);
  }

  ///
  /// this method handles locking on the given object.
  ///
  int			PIG::Lock(const char*			path,
				  struct fuse_file_info*	info,
				  int				command,
				  struct flock*			flags)
  {
    printf("[XXX] %s(%s, 0x%x, %u, 0x%x)\n",
	   __FUNCTION__,
	   path, info, command, flags);

    // XXX: to implement

    return (0);
  }

  ///
  /// this method creates a symbolic link.
  ///
  int			PIG::Symlink(const char*		target,
				     const char*		source)
  {
    etoile::context::Identifier	directory;
    etoile::context::Identifier	link;
    etoile::path::Slice		name;
    etoile::path::Way		from(etoile::path::Way(source), name);
    etoile::path::Way		to(target);

    printf("[XXX] %s(%s, %s)\n",
	   __FUNCTION__,
	   target, source);

    // create a link
    if (PIG::Channel.Call(
	  Inputs<TagLinkCreate>(),
	  Outputs<TagIdentifier>(link)) == StatusError)
      error(ENOENT);

    // bind the link.
    if (PIG::Channel.Call(
	  Inputs<TagLinkBind>(link, to),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    // load the directory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryLoad>(from),
	  Outputs<TagIdentifier>(directory)) == StatusError)
      error(ENOENT);

    // add an entry for the link.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryAdd>(directory, name, link),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    // store the link.
    if (PIG::Channel.Call(
	  Inputs<TagLinkStore>(link),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    // store the modified directory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryStore>(directory),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    return (0);
  }

  ///
  /// this method returns the target path pointed by the symbolic link.
  ///
  int			PIG::Readlink(const char*		path,
				      char*			buffer,
				      size_t			size)
  {
    etoile::context::Identifier	identifier;
    etoile::path::Way		way(path);
    etoile::path::Way		target;

    printf("[XXX] %s(%s, 0x%x, %u)\n",
	   __FUNCTION__,
	   path, buffer, size);

    // load the link.
    if (PIG::Channel.Call(
	  Inputs<TagLinkLoad>(way),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // resolve the link.
    if (PIG::Channel.Call(
	  Inputs<TagLinkResolve>(identifier),
	  Outputs<TagLinkWay>(target)) == StatusError)
      error(ENOENT);

    // store the link.
    if (PIG::Channel.Call(
	  Inputs<TagLinkStore>(identifier),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    // copy as much as possible of the target into the output buffer.
    ::strncpy(buffer,
	      target.path.c_str(),
	      (target.path.length() + 1) < size ?
	        target.path.length() + 1 :
	        size);

    return (0);
  }

  ///
  /// this method creates a new file and opens it.
  ///
  int			PIG::Create(const char*			path,
				    mode_t			mode,
				    struct fuse_file_info*	info)
  {
    etoile::path::Slice		name;
    etoile::path::Way		way(etoile::path::Way(path), name);
    etoile::context::Identifier	directory;
    etoile::context::Identifier	file;
    etoile::kernel::Permissions	permissions;

    printf("[XXX] %s(%s, 0%o, 0x%x)\n",
	   __FUNCTION__,
	   path, mode, info);

    // load the directory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryLoad>(way),
	  Outputs<TagIdentifier>(directory)) == StatusError)
      error(ENOENT);

    // create the file.
    if (PIG::Channel.Call(
	  Inputs<TagFileCreate>(),
	  Outputs<TagIdentifier>(file)) == StatusError)
      error(EINTR);

    // compute the permissions.
    if ((mode & S_IRUSR) != 0)
      permissions |= etoile::kernel::PermissionRead;

    if ((mode & S_IWUSR) != 0)
      permissions |= etoile::kernel::PermissionWrite;

    // set the owner permissions.
    if (PIG::Channel.Call(
	  Inputs<TagAccessUpdate>(file, PIG::Subject, permissions),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    // add the file to the directory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryAdd>(directory, name, file),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    // store the file, ensuring the file system consistency.
    //
    // indeed, if the file is kept opened and the directory is stored
    // someone could see an incorrect entry. although errors will occur
    // extremely rarely and such errors do not cause harm, especially
    // considering the Infinit consistency guaranties, we still prefer
    // to do things right, at least for now.
    if (PIG::Channel.Call(
	  Inputs<TagFileStore>(file),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    // store the directory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryStore>(directory),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    // finally, the file is reopened.
    if (PIG::Channel.Call(
	  Inputs<TagFileLoad>(etoile::path::Way(path)),
	  Outputs<TagIdentifier>(file)) == StatusError)
      error(ENOENT);

    // store the identifier in the file handle.
    info->fh = (uint64_t)new etoile::context::Identifier(file);

    return (0);
  }

  ///
  /// this method opens a file.
  ///
  int			PIG::Open(const char*			path,
				  struct fuse_file_info*	info)
  {
    etoile::path::Way		way(path);
    etoile::context::Identifier	identifier;

    printf("[XXX] %s(%s, 0x%x)\n",
	   __FUNCTION__,
	   path, info);

    // load the file.
    if (PIG::Channel.Call(
	  Inputs<TagFileLoad>(way),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // store the identifier in the file handle.
    info->fh = (uint64_t)new etoile::context::Identifier(identifier);

    return (0);
  }

  ///
  /// this method writes data to a file.
  ///
  int			PIG::Write(const char*			path,
				   const char*			buffer,
				   size_t			size,
				   off_t			offset,
				   struct fuse_file_info*	info)
  {
    etoile::context::Identifier*	identifier;
    Region				region;

    printf("[XXX] %s(%s, 0x%x, %u, %qu, 0x%x)\n",
	   __FUNCTION__,
	   path, buffer, size, offset, info);

    // retrieve the identifier.
    identifier = (etoile::context::Identifier*)info->fh;

    // wrap the buffer.
    if (region.Wrap((Byte*)buffer, size) == StatusError)
      error(EINTR);

    // write the file.
    if (PIG::Channel.Call(
	  Inputs<TagFileWrite>(*identifier,
			       (etoile::kernel::Offset)offset,
			       region),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    return (size);
  }

  ///
  /// this method reads data from a file.
  ///
  int			PIG::Read(const char*			path,
				  char*				buffer,
				  size_t			size,
				  off_t				offset,
				  struct fuse_file_info*	info)
  {
    etoile::context::Identifier*	identifier;
    Region				region;

    printf("[XXX] %s(%s, 0x%x, %u, %qu, 0x%x)\n",
	   __FUNCTION__,
	   path, buffer, size, offset, info);

    // retrieve the identifier.
    identifier = (etoile::context::Identifier*)info->fh;

    // read the file.
    if (PIG::Channel.Call(
	  Inputs<TagFileRead>(*identifier,
			      (etoile::kernel::Offset)offset,
			      (etoile::kernel::Size)size),
	  Outputs<TagFileRegion>(region)) == StatusError)
      error(ENOENT);

    // copy the data to the output buffer.
    ::memcpy(buffer, region.contents, region.size);

    return ((int)region.size);
  }

  ///
  /// this method modifies the size of a file.
  ///
  int			PIG::Truncate(const char*		path,
				      off_t			size)
  {
    etoile::context::Identifier	identifier;
    etoile::path::Way		way(path);
    struct fuse_file_info	info;
    int				result;

    printf("[XXX] %s(%s, %qu)\n",
	   __FUNCTION__,
	   path, size);

    // load the file.
    if (PIG::Channel.Call(
	  Inputs<TagFileLoad>(way),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // set the identifier in the fuse_file_info structure.
    info.fh = (uint64_t)&identifier;

    // call the Ftruncate() method.
    result = PIG::Ftruncate(path, size, &info);

    // store the file.
    if (PIG::Channel.Call(
	  Inputs<TagFileStore>(identifier),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    return (result);
  }

  ///
  /// this method modifies the size of an opened file.
  ///
  int			PIG::Ftruncate(const char*		path,
				       off_t			size,
				       struct fuse_file_info*	info)
  {
    etoile::context::Identifier*	identifier;

    printf("[XXX] %s(%s, %qu, info)\n",
	   __FUNCTION__,
	   path, size, info);

    // retrieve the identifier.
    identifier = (etoile::context::Identifier*)info->fh;

    // adjust the file's size.
    if (PIG::Channel.Call(
	  Inputs<TagFileAdjust>(*identifier, size),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    return (0);
  }

  ///
  /// this method closes a file.
  ///
  int			PIG::Release(const char*		path,
				     struct fuse_file_info*	info)
  {
    etoile::path::Way			way(path);
    etoile::context::Identifier*	identifier;

    printf("[XXX] %s(%s, 0x%x)\n",
	   __FUNCTION__,
	   path, info);

    // retrieve the identifier.
    identifier = (etoile::context::Identifier*)info->fh;

    // store the file.
    if (PIG::Channel.Call(
	  Inputs<TagFileStore>(*identifier),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    // delete the identifier.
    delete identifier;

    // reset the file handle.
    info->fh = 0;

    return (0);
  }

  ///
  /// this method renames a file.
  ///
  int			PIG::Rename(const char*			source,
				    const char*			target)
  {
    etoile::path::Slice		f;
    etoile::path::Way		from(etoile::path::Way(source), f);
    etoile::path::Slice		t;
    etoile::path::Way		to(etoile::path::Way(target), t);

    printf("[XXX] %s(%s, %s)\n",
	   __FUNCTION__,
	   source, target);

    // if the source and target directories are identical.
    if (from == to)
      {
	//
	// in this case, the object to move can simply be renamed since
	// the source and target directory are identical.
	//
	etoile::context::Identifier	directory;

	// load the directory.
	if (PIG::Channel.Call(
	      Inputs<TagDirectoryLoad>(from),
	      Outputs<TagIdentifier>(directory)) == StatusError)
	  error(ENOENT);

	// rename the entry from _f_ to _t_.
	if (PIG::Channel.Call(
	      Inputs<TagDirectoryRename>(directory, f, t),
	      Outputs<TagOk>()) == StatusError)
	  error(ENOENT);

	// store the directory.
	if (PIG::Channel.Call(
	      Inputs<TagDirectoryStore>(directory),
	      Outputs<TagOk>()) == StatusError)
	  error(ENOENT);
      }
    else
      {
	//
	// otherwise, the object must be moved from _from_ to _to_.
	//
	// the process goes as follows: the object is loaded, an entry in
	// the _to_ directory is added while the entry in the _from_
	// directory is removed.
	//
	etoile::path::Way		way(source);
	etoile::context::Identifier	object;
	etoile::context::Identifier	directory;

	// load the object even though we don't know its genre as we
	// do not need to know to perform this operation.
	if (PIG::Channel.Call(
	      Inputs<TagObjectLoad>(way),
	      Outputs<TagIdentifier>(object)) == StatusError)
	  error(ENOENT);

	// load the _to_ directory.
	if (PIG::Channel.Call(
	      Inputs<TagDirectoryLoad>(to),
	      Outputs<TagIdentifier>(directory)) == StatusError)
	  error(ENOENT);

	// add an entry.
	if (PIG::Channel.Call(
	      Inputs<TagDirectoryAdd>(directory, t, object),
	      Outputs<TagOk>()) == StatusError)
	  error(EACCES);

	// store the _to_ directory.
	if (PIG::Channel.Call(
	      Inputs<TagDirectoryStore>(directory),
	      Outputs<TagOk>()) == StatusError)
	  error(ENOENT);

	// load the _from_ directory.
	if (PIG::Channel.Call(
	      Inputs<TagDirectoryLoad>(from),
	      Outputs<TagIdentifier>(directory)) == StatusError)
	  error(ENOENT);

	// remove the entry.
	if (PIG::Channel.Call(
	      Inputs<TagDirectoryRemove>(directory, f),
	      Outputs<TagOk>()) == StatusError)
	  error(EACCES);

	// store the _from_ directory.
	if (PIG::Channel.Call(
	      Inputs<TagDirectoryStore>(directory),
	      Outputs<TagOk>()) == StatusError)
	  error(ENOENT);

	// store the object.
	if (PIG::Channel.Call(
	      Inputs<TagObjectStore>(object),
	      Outputs<TagOk>()) == StatusError)
	  error(ENOENT);
      }

    return (0);
  }

  ///
  /// this method removes an existing file.
  ///
  int			PIG::Unlink(const char*			path)
  {
    etoile::path::Slice		name;
    etoile::path::Way		child(path);
    etoile::path::Way		parent(child, name);
    etoile::context::Identifier	directory;
    etoile::context::Identifier	identifier;
    etoile::wall::State		state;

    printf("[XXX] %s(%s)\n",
	   __FUNCTION__,
	   path);

    // load the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectLoad>(child),
	  Outputs<TagIdentifier>(identifier)) == StatusError)
      error(ENOENT);

    // retrieve information on the object.
    if (PIG::Channel.Call(
	  Inputs<TagObjectInformation>(identifier),
	  Outputs<TagObjectState>(state)) == StatusError)
      error(EINTR);

    // store the object, as no longer needed.
    if (PIG::Channel.Call(
	  Inputs<TagObjectStore>(identifier),
	  Outputs<TagOk>()) == StatusError)
      error(ENOENT);

    // load the directory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryLoad>(parent),
	  Outputs<TagIdentifier>(directory)) == StatusError)
      error(ENOENT);

    // remove the entry.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryRemove>(directory, name),
	  Outputs<TagOk>()) == StatusError)
      error(EACCES);

    // remove the object according to its type: file or link.
    switch (state.genre)
      {
      case etoile::kernel::GenreFile:
	{
	  // load the file.
	  if (PIG::Channel.Call(
	        Inputs<TagFileLoad>(child),
		Outputs<TagIdentifier>(identifier)) == StatusError)
	    error(ENOENT);

	  // destroy the file.
	  if (PIG::Channel.Call(
	        Inputs<TagFileDestroy>(identifier),
		Outputs<TagOk>()) == StatusError)
	    error(EINTR);

	  break;
	}
      case etoile::kernel::GenreLink:
	{
	  // load the link
	  if (PIG::Channel.Call(
	        Inputs<TagLinkLoad>(child),
		Outputs<TagIdentifier>(identifier)) == StatusError)
	    error(ENOENT);

	  // destroy the link.
	  if (PIG::Channel.Call(
	        Inputs<TagLinkDestroy>(identifier),
		Outputs<TagOk>()) == StatusError)
	    error(EINTR);

	  break;
	}
      case etoile::kernel::GenreDirectory:
	{
	  error(EINTR);
	}
      };

    // store the directory.
    if (PIG::Channel.Call(
	  Inputs<TagDirectoryStore>(directory),
	  Outputs<TagOk>()) == StatusError)
      error(EINTR);

    return (0);
  }

  ///
  /// this method forces the updates to be commited.
  ///
  int			PIG::Fsync(const char*			path,
				   int				datasync,
				   struct fuse_file_info*	info)
  {
    printf("[XXX] %s(%s, %u, 0x%x)\n",
	   __FUNCTION__,
	   path, datasync, info);

    // XXX Publish blocs: Journal::Load(), Journal::Publish()

    return (0);
  }

  ///
  /// this method forces thes updates to be commited.
  ///
  int			PIG::Fsyncdir(const char*		path,
				      int			datasync,
				      struct fuse_file_info*	info)
  {
    printf("[XXX] %s(%s, %u, 0x%x)\n",
	   __FUNCTION__,
	   path, datasync, info);

    // XXX Publish blocs: Journal::Load(), Journal::Publish()

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
    // read the public key from the file.
    //
    {
      String		path = PIG::Path + "/public.b64";
      Region		region;
      struct ::stat	stat;
      Integer32		fd;

      // get the file status.
      if (::stat(path.c_str(), &stat) == -1)
	escape("unable to access the public key file");

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

	  escape("unable to read the public key file");
	}

      // close the file.
      ::close(fd);

      // decode the public key.
      if (Base64::Decode(String((const char*)region.contents, region.size),
			 PIG::K) == StatusError)
	escape("unable to decode the public key");
    }

    //
    // read the phrase from the file.
    //
    {
      String		path = PIG::Path + "/phrase.b64";
      Region		region;
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

      // transform the base64 region into a phrase.
      if (Base64::Decode(String((const char*)region.contents, region.size),
			 PIG::Phrase) == StatusError)
	escape("unable to decode the phrase");
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
	    Inputs< etoile::TagWallConnect >(PIG::Phrase),
	    Outputs< etoile::TagOk >()) == StatusError)
	escape("unable to connect to etoile");
    }

    //
    // initialize the 'somebody' entity.
    //
    {
      struct ::passwd*	passwd;

      // retrieve the passwd structure related to the user 'somebody'.
      if ((passwd = ::getpwnam("somebody")) == NULL)
	escape("it seems that the user 'somebody' does not exist");

      // set the uid and gid.
      PIG::Somebody::UID = passwd->pw_uid;
      PIG::Somebody::GID = passwd->pw_gid;
    }

    //
    // load the user/group maps which will be used to translate Infinit
    // user/group identifiers into local identifiers.
    //
    {
      String		users = PIG::Path + "/access/users.map";
      String		groups = PIG::Path + "/access/groups.map";

      // load the users map.
      if (map::Map<PublicKey>::Load(users, PIG::Maps::Users) == StatusError)
	escape("unable to load the users map");

      // load the groups map.
      if (map::Map<etoile::hole::Address>::Load(groups, PIG::Maps::Groups) ==
	  StatusError)
	escape("unable to load the groups map");
    }

    // initialize the fuse operations.
    //
    {
      // operations.statfs: not supported
      operations.getattr = PIG::Getattr;
      operations.fgetattr = PIG::Fgetattr;
      operations.utimens = PIG::Utimens;

      operations.opendir = PIG::Opendir;
      operations.readdir = PIG::Readdir;
      operations.releasedir = PIG::Releasedir;
      operations.mkdir = PIG::Mkdir;
      operations.rmdir = PIG::Rmdir;

      operations.access = PIG::Access;
      operations.chmod = PIG::Chmod;
      operations.chown = PIG::Chown;
      operations.setxattr = PIG::Setxattr;
      operations.getxattr = PIG::Getxattr;
      operations.listxattr = PIG::Listxattr;
      operations.removexattr = PIG::Removexattr;

      operations.lock = PIG::Lock;

      // operations.link: not supported
      operations.readlink = PIG::Readlink;
      operations.symlink = PIG::Symlink;

      operations.create = PIG::Create;
      // operations.mknod: not supported
      operations.open = PIG::Open;
      operations.write = PIG::Write;
      operations.read = PIG::Read;
      operations.truncate = PIG::Truncate;
      operations.ftruncate = PIG::Ftruncate;
      operations.release = PIG::Release;

      operations.unlink = PIG::Unlink;
      operations.rename = PIG::Rename;

      // operations.flush: not supported
      operations.fsync = PIG::Fsync;
      operations.fsyncdir = PIG::Fsyncdir;
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
