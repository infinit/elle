//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/Crux.cc
//
// created       julien quintard   [wed jun  1 09:30:57 2011]
// updated       julien quintard   [sun sep 11 21:12:27 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/Crux.hh>
#include <pig/PIG.hh>
#include <pig/Janitor.hh>

#include <agent/Agent.hh>
#include <etoile/Etoile.hh>

namespace pig
{

  ///
  /// this macro-function makes it easier to return an error from an
  /// upcall, taking care to log the error but also to release the
  /// remaining identifiers.
  ///
#define error(_text_, _errno_, _identifiers_...)			\
  do									\
    {									\
      log(_text_);							\
									\
      Janitor::Clear(_identifiers_);					\
									\
      return (-(_errno_));						\
    } while (false)

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this constant defines the number of directory entries to fetch from
  /// etoile when performing a Readdir().
  ///
  const nucleus::Size			Crux::Range = 128;

//
// ---------- callbacks -------------------------------------------------------
//

  ///
  /// this method returns general-purpose information on the file system
  /// object identified by _path_.
  ///
  int			Crux::Getattr(const char*		path,
				      struct stat*		stat)
  {
    etoile::gear::Identifier	identifier;
    etoile::path::Way		way(path);
    etoile::path::Chemin	chemin;
    struct ::fuse_file_info	info;
    int				result;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %p)\n",
	     __FUNCTION__,
	     path, stat);

    // resolve the path.
    if (etoile::wall::Path::Resolve(way, chemin) == elle::StatusError)
      return (-ENOENT);

    // load the object.
    if (etoile::wall::Object::Load(chemin, identifier) == elle::StatusError)
      error("unable to load the object",
	    ENOENT);

    // set the identifier in the fuse_file_info structure.
    //
    // be careful, the address is local but it is alright since it is
    // used in Fgetattr() only.
    info.fh = reinterpret_cast<uint64_t>(&identifier);

    // call the Fgetattr() method.
    result = Crux::Fgetattr(path, stat, &info);

    // discard the object.
    if (etoile::wall::Object::Discard(identifier) == elle::StatusError)
      error("unable to discard the object",
	    EINTR);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %p)\n",
	     __FUNCTION__,
	     path, stat);

    return (result);
  }

  ///
  /// this method returns general-purpose information on the file system
  /// object identified by _path_.
  ///
  int			Crux::Fgetattr(const char*		path,
				       struct stat*		stat,
				       struct ::fuse_file_info*	info)
  {
    etoile::gear::Identifier*		identifier;
    etoile::miscellaneous::Information	information;
    etoile::path::Way			way(path);
    elle::String*			name;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %p)\n",
	     __FUNCTION__,
	     path, stat);

    // clear the stat structure.
    ::memset(stat, 0x0, sizeof (struct stat));

    // retrieve the identifier.
    identifier = reinterpret_cast<etoile::gear::Identifier*>(info->fh);

    // retrieve information on the object.
    if (etoile::wall::Object::Information(*identifier,
					  information) == elle::StatusError)
      error("unable to retrieve information on the object",
	    EINTR);

    // set the uid by first looking into the users map. if no local user is
    // found, the 'somebody' user is used instead, indicating that the
    // file belongs to someone, with the given permissions, but cannot
    // be mapped to a local user name.
    if (PIG::Dictionary.users.Lookup(information.keys.owner,
				     name) == elle::StatusTrue)
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
    stat->st_size = static_cast<off_t>(information.size);

    // convert the times into time_t structures.
    stat->st_atime = time(NULL);

    if (information.stamps.creation.Get(stat->st_ctime) == elle::StatusError)
      error("unable to convert the time stamps",
	    EINTR);

    if (information.stamps.modification.Get(stat->st_mtime) ==
	elle::StatusError)
      error("unable to convert the time stamps",
	    EINTR);

    // set the mode and permissions.
    switch (information.genre)
      {
      case nucleus::GenreDirectory:
	{
	  // set the object as being a directory.
	  stat->st_mode = S_IFDIR;

	  // if the user has the read permission, allow her to access
	  // and read the directory.
	  if (information.permissions.owner & nucleus::PermissionRead)
	    stat->st_mode |= S_IRUSR | S_IXUSR;

	  // if the user has the write permission, allow her to modify
	  // the directory content.
	  if (information.permissions.owner & nucleus::PermissionWrite)
	    stat->st_mode |= S_IWUSR;

	  break;
	}
      case nucleus::GenreFile:
	{
	  nucleus::Trait*	trait;

	  stat->st_mode = S_IFREG;

	  // if the user has the read permission, allow her to read
	  // the file.
	  if (information.permissions.owner & nucleus::PermissionRead)
	    stat->st_mode |= S_IRUSR;

	  // if the user has the write permission, allow her to modify
	  // the file content.
	  if (information.permissions.owner & nucleus::PermissionWrite)
	    stat->st_mode |= S_IWUSR;

	  // retrieve the attribute.
	  if (etoile::wall::Attributes::Get(*identifier,
					    "posix::exec",
					    trait) == elle::StatusError)
	    error("unable to retrieve an attribute",
		  ENOENT);

	  // check the trait.
	  if ((trait != NULL) &&
	      (trait->value == "true"))
	    {
	      // active the exec bit.
	      stat->st_mode |= S_IXUSR;
	    }

	  break;
	}
      case nucleus::GenreLink:
	{
	  stat->st_mode = S_IFLNK;

	  // if the user has the read permission, allow her to read and
	  // search the linked object.
	  if (information.permissions.owner & nucleus::PermissionRead)
	    stat->st_mode |= S_IRUSR | S_IXUSR;

	  // if the user has the write permission, allow her to modify
	  // the link.
	  if (information.permissions.owner & nucleus::PermissionWrite)
	    stat->st_mode |= S_IWUSR;
 
	  break;
	}
      default:
	{
	  error("unknown genre",
		EINTR);
	}
      }

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %p)\n",
	     __FUNCTION__,
	     path, stat);

    return (0);
  }

  ///
  /// this method changes the access and modification time of the object.
  ///
  int			Crux::Utimens(const char*		path,
				      const struct timespec[2])
  {
    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, ...)\n",
	     __FUNCTION__,
	     path);

    //
    // XXX not supported: do something about it
    //

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, ...)\n",
	     __FUNCTION__,
	     path);

    return (0);
  }

  ///
  /// this method opens the directory _path_.
  ///
  int			Crux::Opendir(const char*		path,
				      struct ::fuse_file_info*	info)
  {
    etoile::gear::Identifier	identifier;
    etoile::path::Way		way(path);
    etoile::path::Chemin	chemin;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %p)\n",
	     __FUNCTION__,
	     path, info);

    // resolve the path.
    if (etoile::wall::Path::Resolve(way, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the directory.
    if (etoile::wall::Directory::Load(chemin, identifier) == elle::StatusError)
      error("unable to load the directory",
	    ENOENT);

    // duplicate the identifier and save it in the info structure's file
    // handle.
    info->fh =
      reinterpret_cast<uint64_t>(new etoile::gear::Identifier(identifier));

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %p)\n",
	     __FUNCTION__,
	     path, info);

    return (0);
  }

  ///
  /// this method reads the directory entries.
  ///
  int			Crux::Readdir(const char*		path,
				      void*			buffer,
				      ::fuse_fill_dir_t		filler,
				      off_t			offset,
				      struct ::fuse_file_info*	info)
  {
    etoile::path::Way			way(path);
    etoile::gear::Identifier*		identifier;
    off_t				next;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %p, %p, %qu, %p)\n",
	     __FUNCTION__,
	     path, buffer, filler, offset, info);

    // set the identifier pointer to the file handle that has been
    // filled by Opendir().
    identifier = reinterpret_cast<etoile::gear::Identifier*>(info->fh);

    // fill the . and .. entries.
    if (offset == 0)
      filler(buffer, ".", NULL, 1);
    if (offset <= 1)
      filler(buffer, "..", NULL, 2);

    // compute the offset of the next entry.
    if (offset < 2)
      next = 3;
    else
      next = offset + 1;

    // adjust the offset since Etoile starts with zero while in POSIX
    // terms, zero and one are used for . and ..
    if (offset > 2)
      offset -= 2;

    while (true)
      {
	nucleus::Range<nucleus::Entry>		range;
	nucleus::Range<nucleus::Entry>::Scoutor	scoutor;

	// read the directory entries.
	if (etoile::wall::Directory::Consult(
	      *identifier,
	      static_cast<nucleus::Offset>(offset),
	      Crux::Range,
	      range) == elle::StatusError)
	  error("unable to retrieve some directory entries",
		EINTR);

	// add the entries by using the filler() function.
	for (scoutor = range.container.begin();
	     scoutor != range.container.end();
	     scoutor++)
	  {
	    nucleus::Entry*	entry = *scoutor;

	    // fill the buffer with filler().
	    if (filler(buffer, entry->name.c_str(), NULL, next) == 1)
	      {
		// debug.
		if (Infinit::Configuration.debug.pig == true)
		  printf("[pig] /%s(%s, %p, %p, %qu, %p)\n",
			 __FUNCTION__,
			 path, buffer, filler, offset, info);

		return (0);
	      }

	    // compute the offset of the next entry.
	    next++;

	    // increment the offset as well.
	    offset++;
	  }

	if (range.container.size() < Crux::Range)
	  break;
      }

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %p, %p, %qu, %p)\n",
	     __FUNCTION__,
	     path, buffer, filler, offset, info);

    return (0);
  }

  ///
  /// this method closes the directory _path_.
  ///
  int			Crux::Releasedir(const char*		path,
					 struct ::fuse_file_info* info)
  {
    etoile::gear::Identifier*	identifier;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %p)\n",
	     __FUNCTION__,
	     path, info);

    // set the identifier pointer to the file handle that has been
    // filled by Opendir().
    identifier = reinterpret_cast<etoile::gear::Identifier*>(info->fh);

    // discard the object.
    if (etoile::wall::Directory::Discard(*identifier) == elle::StatusError)
      error("unable to discard the directory",
	    EINTR);

    // delete the identifier.
    delete identifier;

    // reset the file handle, just to make sure it is not used anymore.
    info->fh = 0;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %p)\n",
	     __FUNCTION__,
	     path, info);

    return (0);
  }

  ///
  /// this method creates a directory.
  ///
  int			Crux::Mkdir(const char*			path,
				    mode_t			mode)
  {
    nucleus::Permissions	permissions = nucleus::PermissionNone;
    etoile::path::Slab		name;
    etoile::path::Way		way(etoile::path::Way(path), name);
    etoile::path::Chemin	chemin;
    etoile::gear::Identifier	directory;
    etoile::gear::Identifier	subdirectory;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, 0%o)\n",
	     __FUNCTION__,
	     path, mode);

    // resolve the path.
    if (etoile::wall::Path::Resolve(way, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the directory.
    if (etoile::wall::Directory::Load(chemin, directory) == elle::StatusError)
      error("unable to load the directory",
	    ENOENT);

    // create the subdirectory.
    if (etoile::wall::Directory::Create(subdirectory) == elle::StatusError)
      error("unable to create the directory",
	    EINTR,
	    directory);

    // compute the permissions.
    if (mode & S_IRUSR)
      permissions |= nucleus::PermissionRead;

    if (mode & S_IWUSR)
      permissions |= nucleus::PermissionWrite;

    // set the owner permissions.
    if (etoile::wall::Access::Grant(subdirectory,
				    agent::Agent::Subject,
				    permissions) == elle::StatusError)
      error("unable to update the access record",
	    EINTR,
	    subdirectory, directory);

    // add the subdirectory.
    if (etoile::wall::Directory::Add(directory,
				     name,
				     subdirectory) == elle::StatusError)
      error("unable to add an entry to the parent directory",
	    EINTR,
	    subdirectory, directory);

    // store the subdirectory.
    if (etoile::wall::Directory::Store(subdirectory) == elle::StatusError)
      error("unable to store the directory",
	    EINTR,
	    directory);

    // store the directory.
    if (etoile::wall::Directory::Store(directory) == elle::StatusError)
      error("unable to store the directory",
	    EINTR);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, 0%o)\n",
	     __FUNCTION__,
	     path, mode);

    return (0);
  }

  ///
  /// this method removes a directory.
  ///
  int			Crux::Rmdir(const char*			path)
  {
    etoile::path::Slab		name;
    etoile::path::Way		child(path);
    etoile::path::Way		parent(child, name);
    etoile::path::Chemin	chemin;
    etoile::gear::Identifier	directory;
    etoile::gear::Identifier	subdirectory;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s)\n",
	     __FUNCTION__,
	     path);

    // resolve the path.
    if (etoile::wall::Path::Resolve(parent, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the directory.
    if (etoile::wall::Directory::Load(chemin, directory) == elle::StatusError)
      error("unable to load the directory",
	    ENOENT);

    // resolve the path.
    if (etoile::wall::Path::Resolve(child, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT,
	    directory);

    // load the subdirectory.
    if (etoile::wall::Directory::Load(chemin,
				      subdirectory) == elle::StatusError)
      error("unable to load the directory",
	    ENOENT,
	    directory);

    // remove the entry.
    if (etoile::wall::Directory::Remove(directory, name) == elle::StatusError)
      error("unable to remove a directory entry",
	    EACCES,
	    subdirectory, directory);

    // store the directory.
    if (etoile::wall::Directory::Store(directory) == elle::StatusError)
      error("unable to store the directory",
	    EINTR,
	    subdirectory);

    // destroy the subdirectory.
    if (etoile::wall::Directory::Destroy(subdirectory) == elle::StatusError)
      error("unable to destroy the directory",
	    EINTR);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s)\n",
	     __FUNCTION__,
	     path);

    return (0);
  }

  ///
  /// this method checks if the current user has the permission to access
  /// the object _path_ for the operations _mask_.
  ///
  int			Crux::Access(const char*		path,
				     int			mask)
  {
    etoile::gear::Identifier		identifier;
    etoile::miscellaneous::Information	information;
    etoile::path::Way			way(path);
    etoile::path::Chemin		chemin;
    nucleus::Record*			record;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, 0%o)\n",
	     __FUNCTION__,
	     path, mask);

    // resolve the path.
    if (etoile::wall::Path::Resolve(way, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the object.
    if (etoile::wall::Object::Load(chemin, identifier) == elle::StatusError)
      error("unable to load the object",
	    ENOENT);

    // retrieve information on the object.
    if (etoile::wall::Object::Information(identifier,
					  information) == elle::StatusError)
      error("unable to retrieve information on the object",
	    EINTR,
	    identifier);

    // retrieve the user's permissions on the object.
    if (etoile::wall::Access::Lookup(identifier,
				     agent::Agent::Subject,
				     record) == elle::StatusError)
      error("unable to retrieve the access record",
	    ENOENT,
	    identifier);

    // check the record.
    if (record == NULL)
      error("the subject does not seem to have been granted access",
	    EACCES,
	    identifier);

    // check if the permissions match the mask for execution.
    if (mask & X_OK)
      {
	switch (information.genre)
	  {
	  case nucleus::GenreDirectory:
	    {
	      // check if the user has the read permission meaning the
	      // exec bit
	      if (!(record->permissions & nucleus::PermissionRead))
		error("the subject does not have the right to access",
		      EACCES,
		      identifier);

	      break;
	    }
	  case nucleus::GenreFile:
	    {
	      nucleus::Trait*	trait;

	      // get the posix::exec attribute
	      if (etoile::wall::Attributes::Get(identifier,
						"posix::exec",
						trait) == elle::StatusError)
		error("unable to retrieve the attribute",
		      ENOENT,
		      identifier);

	      // check the trait.
	      if ((trait != NULL) &&
		  (trait->value != "true"))
		error("the subject does not have the right to execute",
		      EACCES,
		      identifier);

	      break;
	    }
	  case nucleus::GenreLink:
	    {
	      nucleus::Trait*	trait;

	      // get the posix::exec attribute
	      if (etoile::wall::Attributes::Get(identifier,
						"posix::exec",
						trait) == elle::StatusError)
		error("unable ti retrive the attribute",
		      ENOENT,
		      identifier);

	      // check the trait.
	      if ((trait != NULL) &&
		  (trait->value != "true"))
		error("the subject does not have the right to access",
		      EACCES,
		      identifier);

	      break;
	    }
	  }
      }

    // check if the permissions match the mask for reading.
    if (mask & R_OK)
      {
	if (!(record->permissions & nucleus::PermissionRead))
	  error("the subject does not have the right to read",
		EACCES,
		identifier);
      }

    // check if the permissions match the mask for writing.
    if (mask & W_OK)
      {
	if (!(record->permissions & nucleus::PermissionWrite))
	  error("the subject does not have the right to write",
		EACCES,
		identifier);
      }

    // discard the object.
    if (etoile::wall::Object::Discard(identifier) == elle::StatusError)
      error("unable to discard the object",
	    ENOENT);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, 0%o)\n",
	     __FUNCTION__,
	     path, mask);

    return (0);
  }

  ///
  /// this method modifies the permissions on the object.
  ///
  int			Crux::Chmod(const char*			path,
				    mode_t			mode)
  {
    nucleus::Permissions		permissions = nucleus::PermissionNone;
    etoile::gear::Identifier		identifier;
    etoile::path::Way			way(path);
    etoile::path::Chemin		chemin;
    etoile::miscellaneous::Information	information;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, 0%o)\n",
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

    // compute the permissions.
    if (mode & S_IRUSR)
      permissions |= nucleus::PermissionRead;

    if (mode & S_IWUSR)
      permissions |= nucleus::PermissionWrite;

    // resolve the path.
    if (etoile::wall::Path::Resolve(way, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

   // load the object.
    if (etoile::wall::Object::Load(chemin, identifier) == elle::StatusError)
      error("unable to load the object",
	    ENOENT);

    // update the accesses.
    //
    // note that the method assumes that the caller is the object's owner!
    // if not, an error will occur anyway, so why bother checking.
    if (etoile::wall::Access::Grant(identifier,
				    agent::Agent::Subject,
				    permissions) == elle::StatusError)
      error("unable to update the access records",
	    ENOENT,
	    identifier);

    // retrieve information on the object.
    if (etoile::wall::Object::Information(identifier,
					  information) == elle::StatusError)
      error("unable to retrieve information on the object",
	    EINTR,
	    identifier);

    // if the execution bit is to be set...
    if (mode & S_IXUSR)
      {
	// set the posix::exec attribute if necessary i.e depending on the
	// file genre.
	switch (information.genre)
	  {
	  case nucleus::GenreFile:
	    {
	      // set the posix::exec attribute
	      if (etoile::wall::Attributes::Set(identifier,
						"posix::exec",
						"true") == elle::StatusError)
		error("unable to set the attribute",
		      EACCES,
		      identifier);

	      break;
	    }
	  case nucleus::GenreDirectory:
	  case nucleus::GenreLink:
	    {
	      // nothing to do for the other genres.
	      break;
	    }
	  }
      }

    // store the object.
    if (etoile::wall::Object::Store(identifier) == elle::StatusError)
      error("unable to store the object",
	    ENOENT);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, 0%o)\n",
	     __FUNCTION__,
	     path, mode);

    return (0);
  }

  ///
  /// this method modifies the owner of a given object.
  ///
  int			Crux::Chown(const char*			path,
				    uid_t			uid,
				    gid_t			gid)
  {
    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %u, %u)\n",
	     __FUNCTION__,
	     path, uid, gid);

    // XXX to implement.

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %u, %u)\n",
	     __FUNCTION__,
	     path, uid, gid);

    return (0);
  }

#ifdef HAVE_SETXATTR
  ///
  /// this method sets an extended attribute value.
  ///
  /// note that the flags are ignored!
  ///
  int			Crux::Setxattr(const char*		path,
				       const char*		name,
				       const char*		value,
				       size_t			size,
				       int			flags)
  {
    etoile::gear::Identifier	identifier;
    etoile::path::Way		way(path);
    etoile::path::Chemin	chemin;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %s, %p, %u, 0x%x)\n",
	     __FUNCTION__,
	     path, name, value, size, flags);

    // resolve the path.
    if (etoile::wall::Path::Resolve(way, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the object.
    if (etoile::wall::Object::Load(chemin, identifier) == elle::StatusError)
      error("unable to load the object",
	    ENOENT);

    // set the attribute.
    if (etoile::wall::Attributes::Set(identifier,
				      elle::String(name),
				      elle::String(value, size)) == 
	elle::StatusError)
      error("unable to set the attribute",
	    ENOENT,
	    identifier);

    // store the object.
    if (etoile::wall::Object::Store(identifier) == elle::StatusError)
      error("unable to store the object",
	    ENOENT);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %s, %p, %u, 0x%x)\n",
	     __FUNCTION__,
	     path, name, value, size, flags);

    return (0);
  }

  ///
  /// this method returns the attribute associated with the given object.
  ///
  int			Crux::Getxattr(const char*		path,
				       const char*		name,
				       char*			value,
				       size_t			size)
  {
    etoile::gear::Identifier	identifier;
    etoile::path::Way		way(path);
    etoile::path::Chemin	chemin;
    nucleus::Trait*		trait;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %s, %p, %u)\n",
	     __FUNCTION__,
	     path, name, value, size);

    // resolve the path.
    if (etoile::wall::Path::Resolve(way, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the object.
    if (etoile::wall::Object::Load(chemin, identifier) == elle::StatusError)
      error("unable to load the object",
	    ENOENT);

    // get the attribute.
    if (etoile::wall::Attributes::Get(identifier,
				      elle::String(name),
				      trait) == elle::StatusError)
      error("unable to retrieve an attribute",
	    ENOENT,
	    identifier);

    // discard the object.
    if (etoile::wall::Object::Discard(identifier) == elle::StatusError)
      error("unable to discard the object",
	    ENOENT);

    // test if a trait has been found.
    if (trait == NULL)
      error("unable to locate this attribute",
	    ENOATTR);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %s, %p, %u)\n",
	     __FUNCTION__,
	     path, name, value, size);

    // if the size is null, it means that this call must be considered
    // as a request for the size required to store the value.
    if (size == 0)
      {
	return (trait->value.length());
      }
    else
      {
	// otherwise, copy the trait value in the value buffer.
	::memcpy(value, trait->value.data(), trait->value.length());

	// return the length of the value.
	return (trait->value.length());
      }
  }

  ///
  /// this method returns the list of attribute names.
  ///
  int			Crux::Listxattr(const char*		path,
					char*			list,
					size_t			size)
  {
    etoile::gear::Identifier			identifier;
    etoile::path::Way				way(path);
    etoile::path::Chemin			chemin;
    nucleus::Range<nucleus::Trait>		range;
    nucleus::Range<nucleus::Trait>::Scoutor	scoutor;
    size_t					offset;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %p, %u)\n",
	     __FUNCTION__,
	     path, list, size);

    // resolve the path.
    if (etoile::wall::Path::Resolve(way, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the object.
    if (etoile::wall::Object::Load(chemin, identifier) == elle::StatusError)
      error("unable to load the object",
	    ENOENT);

    // fetch the attributes.
    if (etoile::wall::Attributes::Fetch(identifier,
					range) == elle::StatusError)
      error("unable to fetch the attributes",
	    ENOENT,
	    identifier);

    // discard the object.
    if (etoile::wall::Object::Discard(identifier) == elle::StatusError)
      error("unable to discard the object",
	    ENOENT);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %p, %u)\n",
	     __FUNCTION__,
	     path, list, size);

    // if the size is zero, this call must return the size required to
    // store the list.
    if (size == 0)
      {
	for (scoutor = range.container.begin();
	     scoutor != range.container.end();
	     scoutor++)
	  {
	    nucleus::Trait*	trait = *scoutor;

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
	    nucleus::Trait*	trait = *scoutor;

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
  int			Crux::Removexattr(const char*		path,
					  const char*		name)
  {
    etoile::gear::Identifier	identifier;
    etoile::path::Way		way(path);
    etoile::path::Chemin	chemin;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %s)\n",
	     __FUNCTION__,
	     path, name);

    // resolve the path.
    if (etoile::wall::Path::Resolve(way, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the object.
    if (etoile::wall::Object::Load(chemin, identifier) == elle::StatusError)
      error("unable to load the object",
	    ENOENT);

    // omit the attribute.
    if (etoile::wall::Attributes::Omit(identifier,
				       elle::String(name)) ==
	elle::StatusError)
      error("unable to omit the attributes",
	    ENOENT,
	    identifier);

    // store the object.
    if (etoile::wall::Object::Store(identifier) == elle::StatusError)
      error("unable to store the object",
	    ENOENT);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %s)\n",
	     __FUNCTION__,
	     path, name);

    return (0);
  }
#endif

  ///
  /// this method creates a symbolic link.
  ///
  int			Crux::Symlink(const char*		target,
				      const char*		source)
  {
    etoile::gear::Identifier	directory;
    etoile::gear::Identifier	link;
    etoile::path::Slab		name;
    etoile::path::Way		from(etoile::path::Way(source), name);
    etoile::path::Way		to(target);
    etoile::path::Chemin	chemin;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %s)\n",
	     __FUNCTION__,
	     target, source);

    // resolve the path.
    if (etoile::wall::Path::Resolve(from, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the directory.
    if (etoile::wall::Directory::Load(chemin, directory) == elle::StatusError)
      error("unable to load the directory",
	    ENOENT);

    // create a link
    if (etoile::wall::Link::Create(link) == elle::StatusError)
      error("unable to create a link",
	    ENOENT,
	    directory);

    // bind the link.
    if (etoile::wall::Link::Bind(link, to) == elle::StatusError)
      error("unable to bind the link",
	    ENOENT,
	    link, directory);

    // add an entry for the link.
    if (etoile::wall::Directory::Add(directory,
				     name,
				     link) == elle::StatusError)
      error("unable to add an entry to the directory",
	    ENOENT,
	    link, directory);

    // store the link.
    if (etoile::wall::Link::Store(link) == elle::StatusError)
      error("unable to store the link",
	    ENOENT,
	    directory);

    // store the modified directory.
    if (etoile::wall::Directory::Store(directory) == elle::StatusError)
      error("unable to store the directory",
	    ENOENT);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %s)\n",
	     __FUNCTION__,
	     target, source);

    return (0);
  }

  ///
  /// this method returns the target path pointed by the symbolic link.
  ///
  int			Crux::Readlink(const char*		path,
				       char*			buffer,
				       size_t			size)
  {
    etoile::gear::Identifier	identifier;
    etoile::path::Way		way(path);
    etoile::path::Chemin	chemin;
    etoile::path::Way		target;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %p, %u)\n",
	     __FUNCTION__,
	     path, buffer, size);

    // resolve the path.
    if (etoile::wall::Path::Resolve(way, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the link.
    if (etoile::wall::Link::Load(chemin, identifier) == elle::StatusError)
      error("unable to load the link",
	    ENOENT);

    // resolve the link.
    if (etoile::wall::Link::Resolve(identifier, target) == elle::StatusError)
      error("unable to resolve the link",
	    ENOENT,
	    identifier);

    // discard the link.
    if (etoile::wall::Link::Discard(identifier) == elle::StatusError)
      error("unable to discard the link",
	    ENOENT);

    // copy as much as possible of the target into the output buffer.
    ::strncpy(buffer,
	      target.path.c_str(),
	      (target.path.length() + 1) < size ?
	        target.path.length() + 1 :
	        size);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %p, %u)\n",
	     __FUNCTION__,
	     path, buffer, size);

    return (0);
  }

  ///
  /// this method creates a new file and opens it.
  ///
  int			Crux::Create(const char*		path,
				     mode_t			mode,
				     struct ::fuse_file_info*	info)
  {
    nucleus::Permissions	permissions = nucleus::PermissionNone;
    etoile::path::Slab		name;
    etoile::path::Way		way(etoile::path::Way(path), name);
    etoile::path::Chemin	chemin;
    etoile::gear::Identifier	directory;
    etoile::gear::Identifier	file;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, 0%o, %p)\n",
	     __FUNCTION__,
	     path, mode, info);

    // resolve the path.
    if (etoile::wall::Path::Resolve(way, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the directory.
    if (etoile::wall::Directory::Load(chemin, directory) == elle::StatusError)
      error("unable to load the directory",
	    ENOENT);

    // create the file.
    if (etoile::wall::File::Create(file) == elle::StatusError)
      error("unable to create a file",
	    EINTR,
	    directory);

    // compute the permissions.
    if (mode & S_IRUSR)
      permissions |= nucleus::PermissionRead;

    if (mode & S_IWUSR)
      permissions |= nucleus::PermissionWrite;

    // set the owner permissions.
    if (etoile::wall::Access::Grant(file,
				    agent::Agent::Subject,
				    permissions) == elle::StatusError)
      error("unable to update the access records",
	    EINTR,
	    file, directory);

    // if the file has the exec bit, add the posix::exec attribute.
    if (mode & S_IXUSR)
      {
	// set the posix::exec attribute
	if (etoile::wall::Attributes::Set(file,
					  "posix::exec",
					  "true") == elle::StatusError)
	  error("unable to set the attributes",
		EACCES,
		file, directory);
      }

    // add the file to the directory.
    if (etoile::wall::Directory::Add(directory,
				     name,
				     file) == elle::StatusError)
      error("unable to add an entry to the directory",
	    EINTR,
	    file, directory);

    // store the file, ensuring the file system consistency.
    //
    // indeed, if the file is kept opened and the directory is stored
    // someone could see an incorrect entry. although errors will occur
    // extremely rarely and such errors do not cause harm, especially
    // considering the Infinit consistency guaranties, we still prefer
    // to do things right, at least for now.
    if (etoile::wall::File::Store(file) == elle::StatusError)
      error("unable to store the file",
	    EINTR, directory);

    // store the directory.
    if (etoile::wall::Directory::Store(directory) == elle::StatusError)
      error("unable to store the directory",
	    EINTR);

    // resolve the path.
    if (etoile::wall::Path::Resolve(etoile::path::Way(path),
				    chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // finally, the file is reopened.
    if (etoile::wall::File::Load(chemin, file) == elle::StatusError)
      error("unable to load the file",
	    ENOENT);

    // store the identifier in the file handle.
    info->fh =
      reinterpret_cast<uint64_t>(new etoile::gear::Identifier(file));

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, 0%o, %p)\n",
	     __FUNCTION__,
	     path, mode, info);

    return (0);
  }

  ///
  /// this method opens a file.
  ///
  int			Crux::Open(const char*			path,
				   struct ::fuse_file_info*	info)
  {
    etoile::path::Way		way(path);
    etoile::path::Chemin	chemin;
    etoile::gear::Identifier	identifier;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %p)\n",
	     __FUNCTION__,
	     path, info);

    // resolve the path.
    if (etoile::wall::Path::Resolve(way, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the file.
    if (etoile::wall::File::Load(chemin, identifier) == elle::StatusError)
      error("unable to load the file",
	    ENOENT);

    // store the identifier in the file handle.
    info->fh =
      reinterpret_cast<uint64_t>(new etoile::gear::Identifier(identifier));

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %p)\n",
	     __FUNCTION__,
	     path, info);

    return (0);
  }

  ///
  /// this method writes data to a file.
  ///
  int			Crux::Write(const char*			path,
				    const char*			buffer,
				    size_t			size,
				    off_t			offset,
				    struct ::fuse_file_info*	info)
  {
    etoile::gear::Identifier*	identifier;
    elle::Region		region;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %p, %u, %qu, %p)\n",
	     __FUNCTION__,
	     path, buffer, size, offset, info);

    // retrieve the identifier.
    identifier = reinterpret_cast<etoile::gear::Identifier*>(info->fh);

    // wrap the buffer.
    if (region.Wrap(reinterpret_cast<const elle::Byte*>(buffer),
		    size) == elle::StatusError)
      error("unable to wrap the buffer",
	    EINTR);

    // write the file.
    if (etoile::wall::File::Write(*identifier,
				  static_cast<nucleus::Offset>(offset),
				  region) == elle::StatusError)
      error("unable to write the file",
	    EACCES);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %p, %u, %qu, %p)\n",
	     __FUNCTION__,
	     path, buffer, size, offset, info);

    return (size);
  }

  ///
  /// this method reads data from a file.
  ///
  int			Crux::Read(const char*			path,
				   char*			buffer,
				   size_t			size,
				   off_t			offset,
				   struct ::fuse_file_info*	info)
  {
    etoile::gear::Identifier*	identifier;
    elle::Region		region;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %p, %u, %qu, %p)\n",
	     __FUNCTION__,
	     path, buffer, size, offset, info);

    // retrieve the identifier.
    identifier = reinterpret_cast<etoile::gear::Identifier*>(info->fh);

    // read the file.
    if (etoile::wall::File::Read(*identifier,
				 static_cast<nucleus::Offset>(offset),
				 static_cast<nucleus::Size>(size),
				 region) == elle::StatusError)
      error("unable to read the file",
	    EACCES);

    // copy the data to the output buffer.
    ::memcpy(buffer, region.contents, region.size);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %p, %u, %qu, %p)\n",
	     __FUNCTION__,
	     path, buffer, size, offset, info);

    return (region.size);
  }

  ///
  /// this method modifies the size of a file.
  ///
  int			Crux::Truncate(const char*		path,
				       off_t			size)
  {
    etoile::gear::Identifier	identifier;
    etoile::path::Way		way(path);
    etoile::path::Chemin	chemin;
    struct ::fuse_file_info	info;
    int				result;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %qu)\n",
	     __FUNCTION__,
	     path, size);

    // resolve the path.
    if (etoile::wall::Path::Resolve(way, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the file.
    if (etoile::wall::File::Load(chemin, identifier) == elle::StatusError)
      error("unable to load the file",
	    ENOENT);

    // set the identifier in the fuse_file_info structure.
    info.fh = reinterpret_cast<uint64_t>(&identifier);

    // call the Ftruncate() method.
    result = Crux::Ftruncate(path, size, &info);

    // store the file.
    if (etoile::wall::File::Store(identifier) == elle::StatusError)
      error("unable to store the file",
	    EINTR);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %qu)\n",
	     __FUNCTION__,
	     path, size);

    return (result);
  }

  ///
  /// this method modifies the size of an opened file.
  ///
  int			Crux::Ftruncate(const char*		path,
					off_t			size,
					struct ::fuse_file_info* info)
  {
    etoile::gear::Identifier*	identifier;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %qu, %p)\n",
	     __FUNCTION__,
	     path, size, info);

    // retrieve the identifier.
    identifier = reinterpret_cast<etoile::gear::Identifier*>(info->fh);

    // adjust the file's size.
    if (etoile::wall::File::Adjust(*identifier,
				   size) == elle::StatusError)
      error("unable to adjust the size of the file",
	    ENOENT);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %qu, %p)\n",
	     __FUNCTION__,
	     path, size, info);

    return (0);
  }

  ///
  /// this method closes a file.
  ///
  int			Crux::Release(const char*		path,
				      struct ::fuse_file_info*	info)
  {
    etoile::path::Way		way(path);
    etoile::gear::Identifier*	identifier;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %p)\n",
	     __FUNCTION__,
	     path, info);

    // retrieve the identifier.
    identifier = reinterpret_cast<etoile::gear::Identifier*>(info->fh);

    // store the file.
    if (etoile::wall::File::Store(*identifier) == elle::StatusError)
      error("unable to store the file",
	    ENOENT);

    // delete the identifier.
    delete identifier;

    // reset the file handle.
    info->fh = 0;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %p)\n",
	     __FUNCTION__,
	     path, info);

    return (0);
  }

  ///
  /// this method renames a file.
  ///
  int			Crux::Rename(const char*		source,
				     const char*		target)
  {
    etoile::path::Slab		f;
    etoile::path::Way		from(etoile::path::Way(source), f);
    etoile::path::Slab		t;
    etoile::path::Way		to(etoile::path::Way(target), t);
    etoile::gear::Identifier	object;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s, %s)\n",
	     __FUNCTION__,
	     source, target);

    // if the source and target directories are identical.
    if (from == to)
      {
	//
	// in this case, the object to move can simply be renamed since
	// the source and target directory are identical.
	//
	etoile::path::Chemin		chemin;
	etoile::gear::Identifier	directory;

	// resolve the path.
	if (etoile::wall::Path::Resolve(from, chemin) == elle::StatusError)
	  error("unable to resolve the path",
		ENOENT);

	// load the directory.
	if (etoile::wall::Directory::Load(chemin,
					  directory) == elle::StatusError)
	  error("unable to load the directory",
		ENOENT);

	// rename the entry from _f_ to _t_.
	if (etoile::wall::Directory::Rename(directory,
					    f,
					    t) == elle::StatusError)
	  error("unable to rename a directory entry",
		ENOENT,
		directory);

	// store the directory.
	if (etoile::wall::Directory::Store(directory) == elle::StatusError)
	  error("unable to store the directory",
		ENOENT);
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
	etoile::path::Chemin		chemin;
	etoile::gear::Identifier	object;
	etoile::gear::Identifier	directory;

	// resolve the path.
	if (etoile::wall::Path::Resolve(way, chemin) == elle::StatusError)
	  error("unable to resolve the path",
		ENOENT);

	// load the object even though we don't know its genre as we
	// do not need to know to perform this operation.
	if (etoile::wall::Object::Load(chemin, object) == elle::StatusError)
	  error("unable to load the object",
		ENOENT);

	// resolve the path.
	if (etoile::wall::Path::Resolve(to, chemin) == elle::StatusError)
	  error("unable to resolve the path",
		ENOENT,
		object);

	// load the _to_ directory.
	if (etoile::wall::Directory::Load(chemin,
					  directory) == elle::StatusError)
	  error("unable to load the directory",
		ENOENT,
		object);

	// add an entry.
	if (etoile::wall::Directory::Add(directory,
					 t,
					 object) == elle::StatusError)
	  error("unable to add an entry to the directory",
		EACCES,
		directory, object);

	// store the _to_ directory.
	if (etoile::wall::Directory::Store(directory) == elle::StatusError)
	  error("unable to store the directory",
		ENOENT,
		object);

	// resolve the path.
	if (etoile::wall::Path::Resolve(from, chemin) == elle::StatusError)
	  error("unable to resolve the path",
		ENOENT,
		object);

	// load the _from_ directory.
	if (etoile::wall::Directory::Load(chemin,
					  directory) == elle::StatusError)
	  error("unable to load the directory",
		ENOENT,
		object);

	// remove the entry.
	if (etoile::wall::Directory::Remove(directory, f) == elle::StatusError)
	  error("unable to remove a directory entry",
		EACCES,
		directory, object);

	// store the _from_ directory.
	if (etoile::wall::Directory::Store(directory) == elle::StatusError)
	  error("unable to store the directory",
		ENOENT,
		object);

	// store the object.
	if (etoile::wall::Object::Store(object) == elle::StatusError)
	  error("unable to store the object",
		ENOENT);
      }

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s, %s)\n",
	     __FUNCTION__,
	     source, target);

    return (0);
  }

  ///
  /// this method removes an existing file.
  ///
  int			Crux::Unlink(const char*			path)
  {
    etoile::path::Slab			name;
    etoile::path::Way			child(path);
    etoile::path::Way			parent(child, name);
    etoile::path::Chemin		chemin;
    etoile::gear::Identifier		directory;
    etoile::gear::Identifier		identifier;
    etoile::miscellaneous::Information	information;

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] %s(%s)\n",
	     __FUNCTION__,
	     path);

    // resolve the path.
    if (etoile::wall::Path::Resolve(child, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the object.
    if (etoile::wall::Object::Load(chemin, identifier) == elle::StatusError)
      error("unable to load the object",
	    ENOENT);

    // retrieve information on the object.
    if (etoile::wall::Object::Information(identifier,
					  information) == elle::StatusError)
      error("unable to retrieve information on the object",
	    EINTR,
	    identifier);

    // discard the object, as no longer needed.
    if (etoile::wall::Object::Discard(identifier) == elle::StatusError)
      error("unable to discard the object",
	    ENOENT);

    // resolve the path.
    if (etoile::wall::Path::Resolve(parent, chemin) == elle::StatusError)
      error("unable to resolve the path",
	    ENOENT);

    // load the directory.
    if (etoile::wall::Directory::Load(chemin, directory) == elle::StatusError)
      error("unable to load the directory",
	    ENOENT);

    // remove the object according to its type: file or link.
    switch (information.genre)
      {
      case nucleus::GenreFile:
	{
	  // resolve the path.
	  if (etoile::wall::Path::Resolve(child, chemin) == elle::StatusError)
	    error("unable to resolve the path",
		  ENOENT,
		  directory);

	  // load the object.
	  if (etoile::wall::File::Load(chemin,
				       identifier) == elle::StatusError)
	    error("unable to load the file",
		  ENOENT,
		  directory);

	  // destroy the file.
	  if (etoile::wall::File::Destroy(identifier) == elle::StatusError)
	    error("unable to destroy the file",
		  EINTR,
		  directory);

	  break;
	}
      case nucleus::GenreLink:
	{
	  // resolve the path.
	  if (etoile::wall::Path::Resolve(child, chemin) == elle::StatusError)
	    error("unable to resolve the path",
		  ENOENT,
		  directory);

	  // load the link
	  if (etoile::wall::Link::Load(chemin,
				       identifier) == elle::StatusError)
	    error("unable to load the link",
		  ENOENT,
		  directory);

	  // destroy the link.
	  if (etoile::wall::Link::Destroy(identifier) == elle::StatusError)
	    error("unable to destroy the link",
		  EINTR,
		  directory);

	  break;
	}
      case nucleus::GenreDirectory:
	{
	  error("meaningless operation",
		EINTR);
	}
      };

    // remove the entry.
    if (etoile::wall::Directory::Remove(directory, name) == elle::StatusError)
      error("unable to remove a directory entry",
	    EACCES,
	    directory);

    // store the directory.
    if (etoile::wall::Directory::Store(directory) == elle::StatusError)
      error("unable to store the directory",
	    EINTR);

    // debug.
    if (Infinit::Configuration.debug.pig == true)
      printf("[pig] /%s(%s)\n",
	     __FUNCTION__,
	     path);

    return (0);
  }

}
