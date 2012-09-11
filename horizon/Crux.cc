#include <horizon/Crib.hh>
#include <horizon/Crux.hh>
#include <horizon/Handle.hh>
#include <horizon/Janitor.hh>
#include <horizon/Policy.hh>
#include <horizon/Horizon.hh>

#include <agent/Agent.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/path/Way.hh>
#include <etoile/path/Slab.hh>
#include <etoile/path/Chemin.hh>
#include <etoile/wall/Object.hh>
#include <etoile/wall/File.hh>
#include <etoile/wall/Directory.hh>
#include <etoile/wall/Link.hh>
#include <etoile/wall/Attributes.hh>
#include <etoile/wall/Access.hh>
#include <etoile/wall/Path.hh>
#include <etoile/abstract/Object.hh>

#include <nucleus/neutron/Entry.hh>
#include <nucleus/neutron/Record.hh>
#include <nucleus/neutron/Permissions.hh>
#include <nucleus/neutron/Genre.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/Index.hh>
#include <nucleus/neutron/Offset.hh>
#include <nucleus/neutron/Trait.hh>
#include <nucleus/neutron/Subject.hh>
#include <nucleus/neutron/Range.hh>

#include <hole/Hole.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.horizon.Crux");

namespace horizon
{
  /// This macro-function makes it easier to return an error from an
  /// upcall, taking care to log the error but also to release the
  /// remaining identifiers.
#define error(_text_, _errno_, _identifiers_...)        \
  do                                                    \
    {                                                   \
      log(_text_);                                      \
                                                        \
      Janitor::Clear(_identifiers_);                    \
                                                        \
      return ((_errno_));                               \
    } while (false)

  /// The number of directory entries to fetch from etoile when
  /// performing a Readdir().
  const nucleus::neutron::Size Crux::Range = 128;

  /// General-purpose information on the file system object
  /// identified by _path_.
  int
  Crux::getattr(const char*               path,
                struct ::stat*            stat)
  {
    ELLE_TRACE_SCOPE("%s(%s, %p)", __FUNCTION__, path, stat);

    etoile::path::Way         way(path);
    struct ::fuse_file_info   info;
    int                       result;

    etoile::path::Chemin      chemin;
    try
      {
        ELLE_DEBUG("resolve the path")
          chemin = etoile::wall::Path::resolve(way);
      }
    catch (etoile::wall::NoSuchFileOrDirectory const&)
      {
        return -ENOENT;
      }

    etoile::gear::Identifier  identifier;
    ELLE_DEBUG("load the object")
      identifier = etoile::wall::Object::Load(chemin);

    // Create a local handle.
    Handle handle(Handle::OperationGetattr, identifier);

    // Set the handle in the fuse_file_info structure.  Be careful,
    // the address is local but it is alright since it is used in
    // Fgetattr() only.
    info.fh = reinterpret_cast<uint64_t>(&handle);

    // Call fgetattr().
    if ((result = Crux::fgetattr(path, stat, &info)) < 0)
      error("unable to get information on the given file descriptor",
            result,
            identifier);

    // Discard the object.
    etoile::wall::Object::Discard(identifier);

    return 0;
  }

  /// General-purpose information on the file system object
  /// identified by _path_.
  int
  Crux::fgetattr(const char*              path,
                 struct ::stat*           stat,
                 struct ::fuse_file_info* info)
  {
    ELLE_TRACE_SCOPE("%s(%s, %p)", __FUNCTION__, path, stat);

    Handle*                           handle;
    elle::String*                     name;

    // Clear the stat structure.
    ::memset(stat, 0x0, sizeof (struct ::stat));

    // Retrieve the handle.
    handle = reinterpret_cast<Handle*>(info->fh);

    // Retrieve information on the object.
    etoile::abstract::Object abstract(
      etoile::wall::Object::Information(handle->identifier));

    // Set the uid by first looking into the users map. if no local
    // user is found, the 'somebody' user is used instead,
    // indicating that the file belongs to someone, with the given
    // permissions, but cannot be mapped to a local user name.
    if (Horizon::Dictionary.users.Lookup(abstract.keys.owner,
                                         name) == elle::Status::True)
      {
        // In this case, the object's owner is known locally.
        struct ::passwd*      passwd;

        // Retrieve the passwd structure associated with this name.
        if ((passwd = ::getpwnam(name->c_str())) != nullptr)
          {
            // Set the uid to the local user's.
            stat->st_uid = passwd->pw_uid;
          }
        else
          {
            // If an error occured, set the user to 'somebody'.
            stat->st_uid = Horizon::Somebody::UID;
          }
      }
    else
      {
        // Otherwise, this user is unknown locally, so indicate the
        // system that this object belongs to the 'somebody' user.
        stat->st_uid = Horizon::Somebody::UID;
      }

    // Since Infinit does not have the concept of current group, the
    // group of this object is set to 'somebody'.
    stat->st_gid = Horizon::Somebody::GID;

    // Set the size.
    stat->st_size = static_cast<off_t>(abstract.size);

    // Set the disk usage by assuming the smallest disk unit is 512
    // bytes.  Note however the the optimised size of I/Os is set to
    // 4096.
    stat->st_blksize = 4096;
    stat->st_blocks =
      (stat->st_size / 512) +
      (stat->st_size % 512) > 0 ? 1 : 0;

    // Set the number of hard links to 1 since no hard link exist
    // but the original object.
    stat->st_nlink = 1;

    // Convert the times into time_t structures.
    stat->st_atime = time(nullptr);

    if (abstract.stamps.creation.Get(stat->st_ctime) ==
        elle::Status::Error)
      error("unable to convert the time stamps",
            -EPERM);

    if (abstract.stamps.modification.Get(stat->st_mtime) ==
        elle::Status::Error)
      error("unable to convert the time stamps",
            -EPERM);

    // Set the mode and permissions.
    switch (abstract.genre)
      {
      case nucleus::neutron::GenreDirectory:
        {
          // Set the object as being a directory.
          stat->st_mode = S_IFDIR;

          // If the user has the read permission, allow her to
          // access and read the directory.
          if ((abstract.permissions.owner &
               nucleus::neutron::PermissionRead) ==
              nucleus::neutron::PermissionRead)
            stat->st_mode |= S_IRUSR | S_IXUSR;

          // If the user has the write permission, allow her to
          // modify the directory content.
          if ((abstract.permissions.owner &
               nucleus::neutron::PermissionWrite) ==
              nucleus::neutron::PermissionWrite)
            stat->st_mode |= S_IWUSR;

          break;
        }
      case nucleus::neutron::GenreFile:
        {
          nucleus::neutron::Trait const* trait;

          stat->st_mode = S_IFREG;

          // If the user has the read permission, allow her to read
          // the file.
          if ((abstract.permissions.owner &
               nucleus::neutron::PermissionRead) ==
              nucleus::neutron::PermissionRead)
            stat->st_mode |= S_IRUSR;

          // If the user has the write permission, allow her to
          // modify the file content.
          if ((abstract.permissions.owner &
               nucleus::neutron::PermissionWrite) ==
              nucleus::neutron::PermissionWrite)
            stat->st_mode |= S_IWUSR;

          // Retrieve the attribute.
          if (etoile::wall::Attributes::Get(handle->identifier,
                                            "perm::exec",
                                            trait) == elle::Status::Error)
            error("unable to retrieve an attribute",
                  -EPERM);

          // Check the trait.
          if ((trait != nullptr) &&
              (trait->value == "true"))
            {
              // Active the exec bit.
              stat->st_mode |= S_IXUSR;
            }

          break;
        }
      case nucleus::neutron::GenreLink:
        {
          stat->st_mode = S_IFLNK;

          // If the user has the read permission, allow her to read
          // and search the linked object.
          if ((abstract.permissions.owner &
               nucleus::neutron::PermissionRead) ==
              nucleus::neutron::PermissionRead)
            stat->st_mode |= S_IRUSR | S_IXUSR;

          // If the user has the write permission, allow her to
          // modify the link.
          if ((abstract.permissions.owner &
               nucleus::neutron::PermissionWrite) ==
              nucleus::neutron::PermissionWrite)
            stat->st_mode |= S_IWUSR;

          break;
        }
      default:
        {
          error("unknown genre",
                -EIO);
        }
      }

    return (0);
  }

  /// This method changes the access and modification time of the
  /// object.
  int
  Crux::utimens(const char* path,
                const struct ::timespec[2])
  {
    ELLE_TRACE_SCOPE("%s(%s, ...)", __FUNCTION__, path);

    // XXX not supported: do something about it

    return (0);
  }

  /// This method opens the directory _path_.
  int
  Crux::opendir(const char* path,
                struct ::fuse_file_info* info)
  {
    ELLE_TRACE_SCOPE("%s(%s, %p)", __FUNCTION__, path, info);

    etoile::gear::Identifier  identifier;
    etoile::path::Way         way(path);

    // Resolve the path.
    etoile::path::Chemin chemin(etoile::wall::Path::resolve(way));

    // Load the directory.
    if (etoile::wall::Directory::Load(chemin,
                                      identifier) == elle::Status::Error)
      error("unable to load the directory",
            -ENOENT);

    // Duplicate the identifier and save it in the info structure's
    // file handle.
    info->fh =
      reinterpret_cast<uint64_t>(new Handle(Handle::OperationOpendir,
                                            identifier));

    return (0);
  }

  /// This method reads the directory entries.
  int
  Crux::readdir(const char* path,
                void* buffer,
                ::fuse_fill_dir_t filler,
                off_t offset,
                struct ::fuse_file_info* info)
  {
    ELLE_TRACE_SCOPE("%s(%s, %p, %p, %d, %p)",
                     __FUNCTION__, path, buffer, filler,
                     static_cast<elle::Natural64>(offset), info);

    Handle*           handle;
    off_t             next;

    // Set the handle pointer to the file handle that has been
    // filled by Opendir().
    handle = reinterpret_cast<Handle*>(info->fh);

    // Check the subject's permissions on the object.
    {
      nucleus::neutron::Record record(
        etoile::wall::Access::lookup(handle->identifier,
                                     agent::Agent::Subject));

      if ((record == nucleus::neutron::Record::Null) ||
          ((record.permissions & nucleus::neutron::PermissionRead) !=
           nucleus::neutron::PermissionRead))
        error("the subject does not have the right to read the "
              "directory entries",
              -EACCES);
    }

    // Fill the . and .. entries.
    if (offset == 0)
      filler(buffer, ".", nullptr, 1);
    if (offset <= 1)
      filler(buffer, "..", nullptr, 2);

    // Compute the offset of the next entry.
    if (offset < 2)
      next = 3;
    else
      next = offset + 1;

    // Adjust the offset since Etoile starts with zero while in
    // POSIX terms, zero and one are used for '.' and '..'.
    if (offset > 2)
      offset -= 2;

    while (true)
      {
        nucleus::neutron::Range<nucleus::neutron::Entry> range;
        nucleus::neutron::Range<nucleus::neutron::Entry>::Scoutor scoutor;

        // Read the directory entries.
        if (etoile::wall::Directory::Consult(
              handle->identifier,
              static_cast<nucleus::neutron::Index>(offset),
              Crux::Range,
              range) == elle::Status::Error)
          error("unable to retrieve some directory entries",
                -EPERM);

        // Add the entries by using the filler() function.
        for (scoutor = range.container.begin();
             scoutor != range.container.end();
             scoutor++)
          {
            nucleus::neutron::Entry* entry = *scoutor;

            // Fill the buffer with filler().
            if (filler(buffer, entry->name.c_str(), nullptr, next) == 1)
              return (0);

            // Compute the offset of the next entry.
            next++;

            // Increment the offset as well.
            offset++;
          }

        if (range.container.size() < Crux::Range)
          break;
      }

    return (0);
  }

  /// This method closes the directory _path_.
  int
  Crux::releasedir(const char* path,
                   struct ::fuse_file_info* info)
  {
    ELLE_TRACE_SCOPE("%s(%s, %p)", __FUNCTION__, path, info);

    // Set the handle pointer to the file handle that has been
    // filled by Opendir().
    std::unique_ptr<Handle> handle(reinterpret_cast<Handle*>(info->fh));

    // Discard the object.
    if (etoile::wall::Directory::Discard(
          handle->identifier) == elle::Status::Error)
      error("unable to discard the directory",
            -EPERM);

    // Reset the file handle, just to make sure it is not used
    // anymore.
    info->fh = 0;

    return (0);
  }

  /// This method creates a directory.
  int
  Crux::mkdir(const char* path,
              mode_t mode)
  {
    ELLE_TRACE_SCOPE("%s(%s, 0%o)", __FUNCTION__, path, mode);

    nucleus::neutron::Permissions permissions =
      nucleus::neutron::PermissionNone;
    etoile::path::Slab        name;
    etoile::path::Way         way(etoile::path::Way(path), name);
    etoile::gear::Identifier  directory;
    etoile::gear::Identifier  subdirectory;

    // Resolve the path.
    etoile::path::Chemin chemin(etoile::wall::Path::resolve(way));

    // Load the directory.
    if (etoile::wall::Directory::Load(chemin,
                                      directory) == elle::Status::Error)
      error("unable to load the directory",
            -ENOENT);

    // Retrieve the subject's permissions on the object.
    nucleus::neutron::Record record(
      etoile::wall::Access::lookup(directory, agent::Agent::Subject));

    // Check the record.
    if ((record == nucleus::neutron::Record::Null) ||
        ((record.permissions & nucleus::neutron::PermissionWrite) !=
         nucleus::neutron::PermissionWrite))
      error("the subject does not have the right to create a "
            "subdirectory in this directory",
            -EACCES,
            directory);

    // Create the subdirectory.
    if (etoile::wall::Directory::Create(subdirectory) == elle::Status::Error)
      error("unable to create the directory",
            -EPERM,
            directory);

    // Compute the permissions.
    if (mode & S_IRUSR)
      permissions |= nucleus::neutron::PermissionRead;

    if (mode & S_IWUSR)
      permissions |= nucleus::neutron::PermissionWrite;

    // Set the owner permissions.
    if (etoile::wall::Access::Grant(subdirectory,
                                    agent::Agent::Subject,
                                    permissions) == elle::Status::Error)
      error("unable to update the access record",
            -EPERM,
            subdirectory, directory);

    switch (hole::Hole::Descriptor.policy())
      {
      case horizon::Policy::accessible:
        {
          // grant the read permission to the 'everybody' group.
          if (etoile::wall::Access::Grant(
                subdirectory,
                hole::Hole::Descriptor.everybody_subject(),
                nucleus::neutron::PermissionRead) == elle::Status::Error)
            error("unable to update the access record",
                  -EPERM,
                  subdirectory, directory);

          break;
        }
      case horizon::Policy::editable:
        {
          // XXX
          assert(false && "not yet supported");

          break;
        }
      case horizon::Policy::confidential:
        {
          // Nothing else to do in this case, the file system object
          // remains private to its owner.

          break;
        }
      default:
        {
          error("invalid policy",
                -EIO,
                subdirectory, directory);
        }
      }

    // Add the subdirectory.
    if (etoile::wall::Directory::Add(directory,
                                     name,
                                     subdirectory) == elle::Status::Error)
      error("unable to add an entry to the parent directory",
            -EPERM,
            subdirectory, directory);

    // Store the subdirectory.
    if (etoile::wall::Directory::Store(subdirectory) == elle::Status::Error)
      error("unable to store the directory",
            -EPERM,
            directory);

    // Store the directory.
    if (etoile::wall::Directory::Store(directory) == elle::Status::Error)
      error("unable to store the directory",
            -EPERM);

    return (0);
  }

  /// This method removes a directory.
  int
  Crux::rmdir(const char* path)
  {
    ELLE_TRACE_SCOPE("%s(%s)", __FUNCTION__, path);

    etoile::path::Slab                name;
    etoile::path::Way                 child(path);
    etoile::path::Way                 parent(child, name);
    etoile::gear::Identifier          directory;
    etoile::gear::Identifier          subdirectory;
    nucleus::neutron::Subject subject;

    // Resolve the path.
    etoile::path::Chemin chemin_parent(etoile::wall::Path::resolve(parent));

    // Load the directory.
    if (etoile::wall::Directory::Load(chemin_parent,
                                      directory) == elle::Status::Error)
      error("unable to load the directory",
            -ENOENT);

    // Retrieve the subject's permissions on the object.
    nucleus::neutron::Record record(
      etoile::wall::Access::lookup(directory, agent::Agent::Subject));

    // Check the record.
    if ((record == nucleus::neutron::Record::Null) ||
        ((record.permissions & nucleus::neutron::PermissionWrite) !=
         nucleus::neutron::PermissionWrite))
      error("the subject does not have the right to remove "
            "a subdirectory from this directory",
            -EACCES,
            directory);

    // Resolve the path.
    etoile::path::Chemin chemin_child(etoile::wall::Path::resolve(child));

    // Load the subdirectory.
    if (etoile::wall::Directory::Load(chemin_child,
                                      subdirectory) == elle::Status::Error)
      error("unable to load the directory",
            -ENOENT,
            directory);

    // Retrieve information on the object.
    etoile::abstract::Object abstract(
      etoile::wall::Object::Information(subdirectory));

    // Create a temporary subject based on the object owner's key.
    if (subject.Create(abstract.keys.owner) == elle::Status::Error)
      error("unable to create a temporary subject",
            -EPERM,
            subdirectory, directory);

    // Check that the subject is the owner of the object.
    if (agent::Agent::Subject != subject)
      error("the subject does not have the right to destroy "
            "this directory",
            -EACCES,
            subdirectory, directory);

    // Remove the entry.
    if (etoile::wall::Directory::Remove(directory,
                                        name) == elle::Status::Error)
      error("unable to remove a directory entry",
            -EPERM,
            subdirectory, directory);

    // Store the directory.
    if (etoile::wall::Directory::Store(directory) == elle::Status::Error)
      error("unable to store the directory",
            -EPERM,
            subdirectory);

    // Destroy the subdirectory.
    if (etoile::wall::Directory::Destroy(subdirectory) == elle::Status::Error)
      error("unable to destroy the directory",
            -EPERM);

    return (0);
  }

  /// This method checks if the current user has the permission to
  /// access the object _path_ for the operations _mask_.
  int
  Crux::access(const char* path,
               int mask)
  {
    ELLE_TRACE_SCOPE("%s(%s, 0%o)", __FUNCTION__, path, mask);

    etoile::path::Way way(path);
    etoile::abstract::Object abstract;

    // Resolve the path.
    etoile::path::Chemin chemin(etoile::wall::Path::resolve(way));

    // Optimisation: if the mask is equal to F_OK i.e there is nothing else
    // to check but the existence of the path, return righ away.
    if (mask == F_OK)
      return (0);

    // Load the object.
    etoile::gear::Identifier identifier(etoile::wall::Object::Load(chemin));

    // Retrieve the user's permissions on the object.
    nucleus::neutron::Record record(
      etoile::wall::Access::lookup(identifier, agent::Agent::Subject));

    // Check the record.
    if (record == nucleus::neutron::Record::Null)
      goto _access;

    // Retrieve information on the object.
    abstract = etoile::wall::Object::Information(identifier);

    // Check if the permissions match the mask for execution.
    if (mask & X_OK)
      {
        switch (abstract.genre)
          {
          case nucleus::neutron::GenreDirectory:
            {
              // Check if the user has the read permission meaning
              // the exec bit
              if ((record.permissions & nucleus::neutron::PermissionRead) !=
                  nucleus::neutron::PermissionRead)
                goto _access;

              break;
            }
          case nucleus::neutron::GenreFile:
            {
              nucleus::neutron::Trait const* trait;

              // Get the perm::exec attribute
              if (etoile::wall::Attributes::Get(identifier,
                                                "perm::exec",
                                                trait) == elle::Status::Error)
                error("unable to retrieve the attribute",
                      -EPERM,
                      identifier);

              // Check the trait.
              if (!((trait != nullptr) &&
                    (trait->value == "true")))
                goto _access;

              break;
            }
          case nucleus::neutron::GenreLink:
            {
              nucleus::neutron::Trait const* trait;

              // Get the perm::exec attribute
              if (etoile::wall::Attributes::Get(identifier,
                                                "perm::exec",
                                                trait) == elle::Status::Error)
                error("unable ti retrive the attribute",
                      -EPERM,
                      identifier);

              // Check the trait.
              if (!((trait != nullptr) &&
                    (trait->value == "true")))
                goto _access;

              break;
            }
          }
      }

    // Check if the permissions match the mask for reading.
    if (mask & R_OK)
      {
        if ((record.permissions & nucleus::neutron::PermissionRead) !=
            nucleus::neutron::PermissionRead)
          goto _access;
      }

    // Check if the permissions match the mask for writing.
    if (mask & W_OK)
      {
        if ((record.permissions & nucleus::neutron::PermissionWrite) !=
            nucleus::neutron::PermissionWrite)
          goto _access;
      }

    // Discard the object.
    etoile::wall::Object::Discard(identifier);

    return (0);

  _access:
    // At this point, the access has been refused.  Therefore, the
    // identifier must be discarded while EACCES must be returned.

    // Discard the identifier.
    etoile::wall::Object::Discard(identifier);

    // Purge the errors.
    purge();

    return (-EACCES);
  }

  /// This method modifies the permissions on the object.
  int
  Crux::chmod(const char* path,
              mode_t mode)
  {
    nucleus::neutron::Permissions permissions =
      nucleus::neutron::PermissionNone;
    etoile::path::Way                 way(path);
    nucleus::neutron::Subject subject;

    ELLE_TRACE_SCOPE("%s(%s, 0%o)", __FUNCTION__, path, mode);

    // Note that this method ignores both the group and other
    // permissions.
    //
    // In order not to ignore them, the system would have to
    // create/update the group entries in the object's access
    // list. although this is completely feasible, it has been
    // decided not to do so because it would incur too much cost.
    //
    // Indeed, on most Unix systems, the umask is set to 022 or is
    // somewhat equivalent, granting permissions, by default, to the
    // default group and the others.
    //
    // Although this is, from our point of view, a very bad idea, it
    // would be catastrophic to create such access records in
    // Infinit, especially because Infinit has been designed and
    // optimised for objects accessed by their sole owners.

    // Compute the permissions.
    if (mode & S_IRUSR)
      permissions |= nucleus::neutron::PermissionRead;

    if (mode & S_IWUSR)
      permissions |= nucleus::neutron::PermissionWrite;

    // Resolve the path.
    etoile::path::Chemin chemin(etoile::wall::Path::resolve(way));

    // Load the object.
    etoile::gear::Identifier identifier(etoile::wall::Object::Load(chemin));

    // Retrieve information on the object.
    etoile::abstract::Object abstract(etoile::wall::Object::Information(identifier));

    // Create a temporary subject based on the object owner's key.
    if (subject.Create(abstract.keys.owner) == elle::Status::Error)
      error("unable to create a temporary subject",
            -EPERM,
            identifier);

    // Check that the subject is the owner of the object.
    if (agent::Agent::Subject != subject)
      error("the subject does not have the right to modify the "
            "access permissions on this object",
            -EACCES,
            identifier);

    // The permission modification must be performed according to
    // the object state.
    //
    // Indeed, if the object has just been created, the permissions
    // assigned at creation will actually be granted when closed.
    //
    // Therefore, should a chmod() be requested between a create()
    // and a close(), the change of permissions should be delayed as
    // it is the case for any file being created.
    //
    // The following therefore checks if the path corresponds to a
    // file in creation. if so, the permissions are recorded for
    // future application.
    if (Crib::Exist(elle::String(path)) == elle::Status::True)
      {
        Handle*       handle;

        // Retrieve the handle, representing the created file, from
        // the crib.  Then update the future permissions in the
        // handle so that, when the file gets closed, these
        // permissions get applied.

        if (Crib::Retrieve(elle::String(path), handle) == elle::Status::Error)
          error("unable to retrieve the handle from the crib",
                -EBADF,
                identifier);

        handle->permissions = permissions;
      }
    else
      {
        // Update the accesses.  Note that the method assumes that
        // the caller is the object's owner! if not, an error will
        // occur anyway, so why bother checking.
        if (etoile::wall::Access::Grant(identifier,
                                        agent::Agent::Subject,
                                        permissions) == elle::Status::Error)
          error("unable to update the access records",
                -EPERM,
                identifier);
      }

    // If the execution bit is to be set...
    if (mode & S_IXUSR)
      {
        // Set the perm::exec attribute if necessary i.e depending
        // on the file genre.
        switch (abstract.genre)
          {
          case nucleus::neutron::GenreFile:
            {
              // Set the perm::exec attribute
              if (etoile::wall::Attributes::Set(identifier,
                                                "perm::exec",
                                                "true") == elle::Status::Error)
                error("unable to set the attribute",
                      -EPERM,
                      identifier);

              break;
            }
          case nucleus::neutron::GenreDirectory:
          case nucleus::neutron::GenreLink:
            {
              // Nothing to do for the other genres.

              break;
            }
          }
      }

    // Store the object.
    etoile::wall::Object::Store(identifier);

    return (0);
  }

  /// This method modifies the owner of a given object.
  int
  Crux::chown(const char* path,
              uid_t uid,
              gid_t gid)
  {
    ELLE_TRACE_SCOPE("%s(%s, %d, %d)", __FUNCTION__, path, uid, gid);

    // Xxx to implement.

    return (0);
  }

#if defined(HAVE_SETXATTR)
  /// This method sets an extended attribute value.
  ///
  /// Note that the flags are ignored!
  int
  Crux::setxattr(const char* path,
                 const char* name,
                 const char* value,
                 size_t size,
                 int flags)
  {
    ELLE_TRACE_SCOPE("%s(%s, %s, %p, %d, 0x%x)",
                     __FUNCTION__, path, name, value, size, flags);

    etoile::path::Way way(path);
    nucleus::neutron::Subject subject;

    // Resolve the path.
    etoile::path::Chemin chemin(etoile::wall::Path::resolve(way));

    // Load the object.
    etoile::gear::Identifier identifier(etoile::wall::Object::Load(chemin));

    // Retrieve information on the object.
    etoile::abstract::Object abstract(etoile::wall::Object::Information(identifier));

    // Create a temporary subject based on the object owner's key.
    if (subject.Create(abstract.keys.owner) == elle::Status::Error)
      error("unable to create a temporary subject",
            -EPERM,
            identifier);

    // Check that the subject is the owner of the object.
    if (agent::Agent::Subject != subject)
      error("the subject does not have the right to modify the attributes "
            "associated with this object",
            -EACCES,
            identifier);

    // Set the attribute.
    if (etoile::wall::Attributes::Set(identifier,
                                      elle::String(name),
                                      elle::String(value, size)) ==
        elle::Status::Error)
      error("unable to set the attribute",
            -EPERM,
            identifier);

    // Store the object.
    etoile::wall::Object::Store(identifier);

    return (0);
  }

  /// This method returns the attribute associated with the given
  /// object.
  int
  Crux::getxattr(const char* path,
                 const char* name,
                 char* value,
                 size_t size)
  {
    ELLE_TRACE_SCOPE("%s(%s, %s, %p, %d)", __FUNCTION__, path, name, value, size);

    etoile::path::Way way(path);
    nucleus::neutron::Trait const* trait;

    // Resolve the path.
    etoile::path::Chemin chemin(etoile::wall::Path::resolve(way));

    // Load the object.
    etoile::gear::Identifier identifier(etoile::wall::Object::Load(chemin));

    // Get the attribute.
    if (etoile::wall::Attributes::Get(identifier,
                                      elle::String(name),
                                      trait) == elle::Status::Error)
      error("unable to retrieve an attribute",
            -EPERM,
            identifier);

    // Discard the object.
    etoile::wall::Object::Discard(identifier);

    // Test if a trait has been found.
    if (trait == nullptr)
      return (-ENOATTR);

    // If the size is null, it means that this call must be
    // considered as a request for the size required to store the
    // value.
    if (size == 0)
      {
        return (trait->value.length());
      }
    else
      {
        // Otherwise, copy the trait value in the value buffer.
        ::memcpy(value, trait->value.data(), trait->value.length());

        // Return the length of the value.
        return (trait->value.length());
      }
  }

  /// This method returns the list of attribute names.
  int
  Crux::listxattr(const char* path,
                  char* list,
                  size_t size)
  {
    ELLE_TRACE_SCOPE("%s(%s, %p, %d)", __FUNCTION__, path, list, size);

    etoile::path::Way way(path);
    nucleus::neutron::Range<nucleus::neutron::Trait> range;
    nucleus::neutron::Range<nucleus::neutron::Trait>::Scoutor scoutor;
    size_t  offset;

    // Resolve the path.
    etoile::path::Chemin chemin(etoile::wall::Path::resolve(way));

    // Load the object.
    etoile::gear::Identifier identifier(etoile::wall::Object::Load(chemin));

    // Fetch the attributes.
    if (etoile::wall::Attributes::Fetch(identifier,
                                        range) == elle::Status::Error)
      error("unable to fetch the attributes",
            -EPERM,
            identifier);

    // Discard the object.
    etoile::wall::Object::Discard(identifier);

    // If the size is zero, this call must return the size required
    // to store the list.
    if (size == 0)
      {
        for (scoutor = range.container.begin();
             scoutor != range.container.end();
             scoutor++)
          {
            nucleus::neutron::Trait* trait = *scoutor;

            // Compute the size.
            size = size + trait->name.length() + 1;
          }

        return (size);
      }
    else
      {
        // Otherwise, go through the attributes and concatenate
        // their names.
        for (scoutor = range.container.begin(), offset = 0;
             scoutor != range.container.end();
             scoutor++)
          {
            nucleus::neutron::Trait* trait = *scoutor;

            // Concatenate the name.
            ::strcpy(list + offset,
                     trait->name.c_str());

            // Adjust the offset.
            offset = offset + trait->name.length() + 1;
          }

        return (offset);
      }
  }

  /// This method removes an attribute.
  int
  Crux::removexattr(const char* path,
                    const char* name)
  {
    ELLE_TRACE_SCOPE("%s(%s, %s)", __FUNCTION__, path, name);

    etoile::path::Way way(path);
    nucleus::neutron::Subject subject;

    // Resolve the path.
    etoile::path::Chemin chemin(etoile::wall::Path::resolve(way));

    // Load the object.
    etoile::gear::Identifier identifier(etoile::wall::Object::Load(chemin));

    // Retrieve information on the object.
    etoile::abstract::Object abstract(etoile::wall::Object::Information(identifier));

    // Create a temporary subject based on the object owner's key.
    if (subject.Create(abstract.keys.owner) == elle::Status::Error)
      error("unable to create a temporary subject",
            -EPERM,
            identifier);

    // Check that the subject is the owner of the object.
    if (agent::Agent::Subject != subject)
      error("the subject does not have the right to modify the attributes "
            "associated with this object",
            -EACCES,
            identifier);

    // Omit the attribute.
    if (etoile::wall::Attributes::Omit(identifier,
                                       elle::String(name)) ==
        elle::Status::Error)
      error("unable to omit the attributes",
            -EPERM,
            identifier);

    // Store the object.
    etoile::wall::Object::Store(identifier);

    return 0;
  }
#endif

  int
  Crux::link(const char* target,
             const char* source)
  {
    ELLE_TRACE_SCOPE("%s(%s, %s)", __FUNCTION__, target, source);

    return -ENOSYS;
  }

  /// This method creates a symbolic link.
  int
  Crux::symlink(const char* target,
                const char* source)
  {
    ELLE_TRACE_SCOPE("%s(%s, %s)", __FUNCTION__, target, source);

    etoile::gear::Identifier  directory;
    etoile::gear::Identifier  link;
    etoile::path::Slab        name;
    etoile::path::Way         from(etoile::path::Way(source), name);
    etoile::path::Way         to(target);

    // Resolve the path.
    etoile::path::Chemin chemin(etoile::wall::Path::resolve(from));

    // Load the directory.
    if (etoile::wall::Directory::Load(chemin,
                                      directory) == elle::Status::Error)
      error("unable to load the directory",
            -ENOENT);

    // Retrieve the subject's permissions on the object.
    nucleus::neutron::Record record(
      etoile::wall::Access::lookup(directory, agent::Agent::Subject));

    // Check the record.
    if ((record == nucleus::neutron::Record::Null) ||
        ((record.permissions & nucleus::neutron::PermissionWrite) !=
         nucleus::neutron::PermissionWrite))
      error("the subject does not have the right to create a link in "
            "this directory",
            -EACCES,
            directory);

    // Create a link
    if (etoile::wall::Link::Create(link) == elle::Status::Error)
      error("unable to create a link",
            -EPERM,
            directory);

    switch (hole::Hole::Descriptor.policy())
      {
      case horizon::Policy::accessible:
        {
          // grant the read permission to the 'everybody' group.
          if (etoile::wall::Access::Grant(
                link,
                hole::Hole::Descriptor.everybody_subject(),
                nucleus::neutron::PermissionRead) == elle::Status::Error)
            error("unable to update the access record",
                  -EPERM,
                  link, directory);

          // grant the exec permission to the 'everybody' group by
          // creating the attribute 'perm::exec'.
          if (etoile::wall::Attributes::Set(link,
                                            "perm::exec",
                                            "true") == elle::Status::Error)
            error("unable to set the attribute",
                  -EPERM,
                  link, directory);

          break;
        }
      case horizon::Policy::editable:
        {
          // XXX
          assert(false && "not yet supported");

          break;
        }
      case horizon::Policy::confidential:
        {
          // Nothing else to do in this case, the file system object
          // remains private to its owner.

          break;
        }
      default:
        {
          error("invalid policy",
                -EIO,
                link, directory);
        }
      }

    // Bind the link.
    if (etoile::wall::Link::Bind(link, to) == elle::Status::Error)
      error("unable to bind the link",
            -EPERM,
            link, directory);

    // Add an entry for the link.
    if (etoile::wall::Directory::Add(directory,
                                     name,
                                     link) == elle::Status::Error)
      error("unable to add an entry to the directory",
            -EPERM,
            link, directory);

    // Store the link.
    if (etoile::wall::Link::Store(link) == elle::Status::Error)
      error("unable to store the link",
            -EPERM,
            directory);

    // Store the modified directory.
    if (etoile::wall::Directory::Store(directory) == elle::Status::Error)
      error("unable to store the directory",
            -EPERM);

    return (0);
  }

  /// This method returns the target path pointed by the symbolic
  /// link.
  int
  Crux::readlink(const char* path,
                 char* buffer,
                 size_t size)
  {
    ELLE_TRACE_SCOPE("%s(%s, %p, %d)", __FUNCTION__, path, buffer,
                     static_cast<elle::Natural64>(size));

    etoile::gear::Identifier  identifier;
    etoile::path::Way         way(path);
    etoile::path::Way         target;

    // Resolve the path.
    etoile::path::Chemin chemin(etoile::wall::Path::resolve(way));

    // Load the link.
    if (etoile::wall::Link::Load(chemin, identifier) == elle::Status::Error)
      error("unable to load the link",
            -ENOENT);

    // Retrieve the subject's permissions on the object.
    nucleus::neutron::Record record(
      etoile::wall::Access::lookup(identifier, agent::Agent::Subject));

    // Check the record.
    if ((record == nucleus::neutron::Record::Null) ||
        ((record.permissions & nucleus::neutron::PermissionRead) ==
         nucleus::neutron::PermissionRead))
      error("the subject does not have the right to read this link",
            -EACCES,
            identifier);

    // Resolve the link.
    if (etoile::wall::Link::Resolve(identifier, target) == elle::Status::Error)
      error("unable to resolve the link",
            -EPERM,
            identifier);

    // Discard the link.
    if (etoile::wall::Link::Discard(identifier) == elle::Status::Error)
      error("unable to discard the link",
            -EPERM);

    // Copy as much as possible of the target into the output
    // buffer.
    ::strncpy(buffer,
              target.path.c_str(),
              (target.path.length() + 1) < size ?
              target.path.length() + 1 :
              size);

    return (0);
  }

  /// This method creates a new file and opens it.
  int
  Crux::create(const char* path,
               mode_t mode,
               struct ::fuse_file_info* info)
  {
    ELLE_TRACE_SCOPE("%s(%s, 0%o, %p)", __FUNCTION__, path, mode, info);

    nucleus::neutron::Permissions permissions =
      nucleus::neutron::PermissionNone;
    etoile::path::Slab        name;
    etoile::path::Way         way(etoile::path::Way(path), name);
    etoile::gear::Identifier  directory;
    etoile::gear::Identifier  file;

    // Resolve the path.
    etoile::path::Chemin chemin(etoile::wall::Path::resolve(way));

    // Load the directory.
    if (etoile::wall::Directory::Load(chemin,
                                      directory) == elle::Status::Error)
      error("unable to load the directory",
            -ENOENT);

    // Retrieve the subject's permissions on the object.
    nucleus::neutron::Record record(
      etoile::wall::Access::lookup(directory, agent::Agent::Subject));

    // Check the record.
    if ((record == nucleus::neutron::Record::Null) ||
        ((record.permissions & nucleus::neutron::PermissionWrite) !=
         nucleus::neutron::PermissionWrite))
      error("the subject does not have the right to create a file in "
            "this directory",
            -EACCES,
            directory);

    // Create the file.
    if (etoile::wall::File::Create(file) == elle::Status::Error)
      error("unable to create a file",
            -EPERM,
            directory);

    // Set default permissions: read and write.
    permissions =
      nucleus::neutron::PermissionRead | nucleus::neutron::PermissionWrite;

    // Set the owner permissions.
    if (etoile::wall::Access::Grant(file,
                                    agent::Agent::Subject,
                                    permissions) == elle::Status::Error)
      error("unable to update the access records",
            -EPERM,
            file, directory);

    // If the file has the exec bit, add the perm::exec attribute.
    if (mode & S_IXUSR)
      {
        // Set the perm::exec attribute
        if (etoile::wall::Attributes::Set(file,
                                          "perm::exec",
                                          "true") == elle::Status::Error)
          error("unable to set the attributes",
                -EPERM,
                file, directory);
      }

    switch (hole::Hole::Descriptor.policy())
      {
      case horizon::Policy::accessible:
        {
          // grant the read permission to the 'everybody' group.
          if (etoile::wall::Access::Grant(
                file,
                hole::Hole::Descriptor.everybody_subject(),
                nucleus::neutron::PermissionRead) == elle::Status::Error)
            error("unable to update the access record",
                  -EPERM,
                  file, directory);

          break;
        }
      case horizon::Policy::editable:
        {
          // XXX
          assert(false && "not yet supported");

          break;
        }
      case horizon::Policy::confidential:
        {
          // Nothing else to do in this case, the file system object
          // remains private to its owner.

          break;
        }
      default:
        {
          error("invalid policy",
                -EIO,
                file, directory);
        }
      }

    // Add the file to the directory.
    if (etoile::wall::Directory::Add(directory,
                                     name,
                                     file) == elle::Status::Error)
      error("unable to add an entry to the directory",
            -EPERM,
            file, directory);

    // Store the file, ensuring the file system consistency.
    //
    // Indeed, if the file is kept opened and the directory is
    // stored someone could see an incorrect entry. although errors
    // will occur extremely rarely and such errors do not cause
    // harm, especially considering the Infinit consistency
    // guaranties, we still prefer to do things right, at least for
    // now.
    if (etoile::wall::File::Store(file) == elle::Status::Error)
      error("unable to store the file",
            -EPERM,
            directory);

    // Store the directory.
    if (etoile::wall::Directory::Store(directory) == elle::Status::Error)
      error("unable to store the directory",
            -EPERM);

    // Resolve the path.
    chemin = etoile::wall::Path::resolve(etoile::path::Way(path));

    // Finally, the file is reopened.
    if (etoile::wall::File::Load(chemin, file) == elle::Status::Error)
      error("unable to load the file",
            -ENOENT);

    // Compute the future permissions as the current ones are
    // temporary.
    permissions = nucleus::neutron::PermissionNone;

    if (mode & S_IRUSR)
      permissions |= nucleus::neutron::PermissionRead;

    if (mode & S_IWUSR)
      permissions |= nucleus::neutron::PermissionWrite;

    // Store the identifier in the file handle.
    info->fh =
      reinterpret_cast<uint64_t>(new Handle(Handle::OperationCreate,
                                            file,
                                            permissions));

    // Add the created and opened file in the crib.
    if (Crib::Add(elle::String(path),
                  reinterpret_cast<Handle*>(info->fh)) == elle::Status::Error)
      error("unable to add the created file to the crib",
            -EBADF);

    return (0);
  }

  /// This method opens a file.
  int
  Crux::open(const char* path,
             struct ::fuse_file_info* info)
  {
    ELLE_TRACE_SCOPE("%s(%s, %p)", __FUNCTION__, path, info);

    etoile::path::Way         way(path);
    etoile::path::Chemin      chemin;
    etoile::gear::Identifier  identifier;

    // Resolve the path.
    chemin = (etoile::wall::Path::resolve(way));

    // Load the file.
    if (etoile::wall::File::Load(chemin, identifier) == elle::Status::Error)
      error("unable to load the file",
            -ENOENT);

    // Store the identifier in the file handle.
    info->fh =
      reinterpret_cast<uint64_t>(new Handle(Handle::OperationOpen,
                                            identifier));

    return (0);
  }

  /// This method writes data to a file.
  int
  Crux::write(const char* path,
              const char* buffer,
              size_t size,
              off_t offset,
              struct ::fuse_file_info* info)
  {
    ELLE_TRACE_SCOPE("%s(%s, %p, %d, %d, %p)",
                     __FUNCTION__, path,
                     reinterpret_cast<const void*>(buffer),
                     static_cast<elle::Natural64>(size),
                     static_cast<elle::Natural64>(offset), info);

    Handle*           handle;
    elle::standalone::Region      region;

    // Retrieve the handle;
    handle = reinterpret_cast<Handle*>(info->fh);

    // Retrieve the subject's permissions on the object.
    nucleus::neutron::Record record(
      etoile::wall::Access::lookup(handle->identifier, agent::Agent::Subject));

    // Check the record.
    if (!((record != nucleus::neutron::Record::Null) &&
          ((record.permissions & nucleus::neutron::PermissionWrite) ==
           nucleus::neutron::PermissionWrite)))
      error("the subject does not have the right to update this file",
            -EACCES);

    // Wrap the buffer.
    if (region.Wrap(reinterpret_cast<const elle::Byte*>(buffer),
                    size) == elle::Status::Error)
      error("unable to wrap the buffer",
            -EPERM);

    // Write the file.
    if (etoile::wall::File::Write(
          handle->identifier,
          static_cast<nucleus::neutron::Offset>(offset),
          region) == elle::Status::Error)
      error("unable to write the file",
            -EPERM);

    return (size);
  }

  /// This method reads data from a file.
  int
  Crux::read(const char* path,
             char* buffer,
             size_t size,
             off_t offset,
             struct ::fuse_file_info* info)
  {
    ELLE_TRACE_SCOPE("%s(%s, %p, %d, %d, %p)",
                     __FUNCTION__, path, buffer,
                     static_cast<elle::Natural64>(size),
                     static_cast<elle::Natural64>(offset), info);

    Handle*           handle;
    elle::standalone::Region      region;

    // Retrieve the handle.
    handle = reinterpret_cast<Handle*>(info->fh);

    // Retrieve the subject's permissions on the object.
    nucleus::neutron::Record record(
      etoile::wall::Access::lookup(handle->identifier, agent::Agent::Subject));

    // Check the record.
    if (!((record != nucleus::neutron::Record::Null) &&
          ((record.permissions & nucleus::neutron::PermissionRead) ==
           nucleus::neutron::PermissionRead)))
      error("the subject does not have the right to read this file",
            -EACCES);

    // Read the file.
    if (etoile::wall::File::Read(
          handle->identifier,
          static_cast<nucleus::neutron::Offset>(offset),
          static_cast<nucleus::neutron::Size>(size),
          region) == elle::Status::Error)
      error("unable to read the file",
            -EPERM);

    // Copy the data to the output buffer.
    ::memcpy(buffer, region.contents, region.size);

    return (region.size);
  }

  /// This method modifies the size of a file.
  int
  Crux::truncate(const char* path,
                 off_t size)
  {
    ELLE_TRACE_SCOPE("%s(%s, %d)", __FUNCTION__, path,
                     static_cast<elle::Natural64>(size));

    etoile::gear::Identifier  identifier;
    etoile::path::Way         way(path);
    struct ::fuse_file_info   info;
    int                       result;

    // Resolve the path.
    etoile::path::Chemin chemin(etoile::wall::Path::resolve(way));

    // Load the file.
    if (etoile::wall::File::Load(chemin, identifier) == elle::Status::Error)
      error("unable to load the file",
            -ENOENT);

    // Create a local handle.
    Handle                    handle(Handle::OperationTruncate,
                                     identifier);

    // Set the handle in the fuse_file_info structure.
    info.fh = reinterpret_cast<uint64_t>(&handle);

    // Call the Ftruncate() method.
    if ((result = Crux::ftruncate(path, size, &info)) < 0)
      error("unable to truncate the given file descriptpr",
            result,
            identifier);

    // Store the file.
    if (etoile::wall::File::Store(identifier) == elle::Status::Error)
      error("unable to store the file",
            -EPERM);

    return (result);
  }

  /// This method modifies the size of an opened file.
  int
  Crux::ftruncate(const char* path,
                  off_t size,
                  struct ::fuse_file_info* info)
  {
    ELLE_TRACE_SCOPE("%s(%s, %d, %p)", __FUNCTION__, path,
                     static_cast<elle::Natural64>(size), info);

    Handle*           handle;

    // Retrieve the handle.
    handle = reinterpret_cast<Handle*>(info->fh);

    // Retrieve the subject's permissions on the object.
    nucleus::neutron::Record record(
      etoile::wall::Access::lookup(handle->identifier, agent::Agent::Subject));

    // Check the record.
    if (!((record != nucleus::neutron::Record::Null) &&
          ((record.permissions & nucleus::neutron::PermissionWrite) ==
           nucleus::neutron::PermissionWrite)))
      error("the subject does not have the right to modify the size of "
            "this file",
            -EACCES);

    // Adjust the file's size.
    if (etoile::wall::File::Adjust(handle->identifier,
                                   size) == elle::Status::Error)
      error("unable to adjust the size of the file",
            -EPERM);

    return (0);
  }

  /// This method closes a file.
  int
  Crux::release(const char* path,
                struct ::fuse_file_info* info)
  {
    ELLE_TRACE_SCOPE("%s(%s, %p)", __FUNCTION__, path, info);

    etoile::path::Way way(path);
    Handle*           handle;

    // Retrieve the handle.
    handle = reinterpret_cast<Handle*>(info->fh);

    // Perform final actions depending on the initial operation.
    switch (handle->operation)
      {
      case Handle::OperationCreate:
        {
          // Remove the created and opened file in the crib.
          if (Crib::Remove(elle::String(path)) == elle::Status::Error)
            error("unable to remove the created file from the crib",
                  -EBADF,
                  handle->identifier);

          // The permissions settings have been delayed in order to
          // support a read-only file being copied in which case a
          // file is created with read-only permissions before being
          // written.
          //
          // Such a normal behaviour would result in runtime
          // permission errors. therefore, the permissions are set
          // once the created file is released in order to overcome
          // this issue.

          // Set the owner permissions.
          if (etoile::wall::Access::Grant(
                handle->identifier,
                agent::Agent::Subject,
                handle->permissions) == elle::Status::Error)
            error("unable to update the access records",
                  -EPERM,
                  handle->identifier);

          break;
        }
      default:
        {
          // Nothing special to do.

          break;
        }
      }

    // Store the file.
    if (etoile::wall::File::Store(handle->identifier) == elle::Status::Error)
      error("unable to store the file",
            -EPERM);

    // Delete the handle.
    delete handle;

    // Reset the file handle.
    info->fh = 0;

    return (0);
  }

  /// This method renames a file.
  int
  Crux::rename(const char* source,
               const char* target)
  {
    ELLE_TRACE_SCOPE("%s(%s, %s)", __FUNCTION__, source, target);

    etoile::path::Slab f;
    etoile::path::Way from(etoile::path::Way(source), f);
    etoile::path::Slab t;
    etoile::path::Way to(etoile::path::Way(target), t);
    etoile::gear::Identifier object;

    // If the source and target directories are identical.
    if (from == to)
      {
        // In this case, the object to move can simply be renamed
        // since the source and target directory are identical.

        etoile::gear::Identifier directory;
        nucleus::neutron::Entry const* entry;
        etoile::path::Chemin chemin;

        ELLE_TRACE("resolve the source directory path")
          chemin = etoile::wall::Path::resolve(from);

        ELLE_TRACE("load source directory")
          if (etoile::wall::Directory::Load(chemin,
                                            directory) == elle::Status::Error)
            error("unable to load the directory",
                  -ENOENT);

        nucleus::neutron::Record record;
        ELLE_TRACE("retrieve the subject's permissions on the object")
          record = etoile::wall::Access::lookup(directory,
                                                agent::Agent::Subject);

        ELLE_TRACE("check the record")
          if (!((record != nucleus::neutron::Record::Null) &&
                ((record.permissions & nucleus::neutron::PermissionWrite) ==
                 nucleus::neutron::PermissionWrite)))
            error("the subject does not have the right to rename this "
                  "directory entry",
                  -EACCES,
                  directory);

        ELLE_TRACE("lookup for the target name")
          if (etoile::wall::Directory::Lookup(directory,
                                              t,
                                              entry) == elle::Status::Error)
            error("unable to lookup the target name",
                  -EPERM,
                  directory);

        // Check if an entry actually exist for the target name
        // meaning that an object is about to get overwritten.
        if (entry != nullptr)
          {
            // In this case, the target object must be destroyed.
            int result;

            // Unlink the object, assuming it is either a file or a
            // link.  Note that the Crux's method is called in order
            // not to have to deal with the target's genre.
            ELLE_TRACE("unlink the existing target %s", target)
              if ((result = Crux::unlink(target)) < 0)
                error("unable to unlink the target object which is "
                      "about to get overwritte",
                      result,
                      directory);
          }

        ELLE_TRACE("rename the entry from %s to %s", f, t)
          if (etoile::wall::Directory::Rename(directory, f, t) ==
              elle::Status::Error)
            error("unable to rename a directory entry",
                  -EPERM,
                  directory);

        ELLE_TRACE("store the directory")
          if (etoile::wall::Directory::Store(directory) == elle::Status::Error)
            error("unable to store the directory",
                  -EPERM);
      }
    else
      {
        // Otherwise, the object must be moved from _from_ to _to_.
        //
        // The process goes as follows: the object is loaded, an
        // entry in the _to_ directory is added while the entry in
        // the _from_ directory is removed.
        etoile::path::Way way(source);
        struct
        {
          etoile::gear::Identifier object;
          etoile::gear::Identifier from;
          etoile::gear::Identifier to;
        } identifier;
        nucleus::neutron::Entry const* entry;

        // Resolve the path.
        etoile::path::Chemin chemin(etoile::wall::Path::resolve(way));

        // Load the object even though we don't know its genre as we
        // do not need to know to perform this operation.
        identifier.object = etoile::wall::Object::Load(chemin);

        // Resolve the path.
        chemin = etoile::wall::Path::resolve(to);

        // Load the _to_ directory.
        if (etoile::wall::Directory::Load(
              chemin,
              identifier.to) == elle::Status::Error)
          error("unable to load the directory",
                -ENOENT,
                identifier.object);

        // Retrieve the subject's permissions on the object.
        nucleus::neutron::Record record(
          etoile::wall::Access::lookup(identifier.to, agent::Agent::Subject));

        // Check the record.
        if (!((record != nucleus::neutron::Record::Null) &&
              ((record.permissions & nucleus::neutron::PermissionWrite) ==
               nucleus::neutron::PermissionWrite)))
          error("the subject does not have the right to rename this "
                "directory entry",
                -EACCES,
                identifier.object, identifier.to);

        // Resolve the path.
        chemin = etoile::wall::Path::resolve(from);

        // Load the _from_ directory.
        if (etoile::wall::Directory::Load(
              chemin,
              identifier.from) == elle::Status::Error)
          error("unable to load the directory",
                -ENOENT,
                identifier.object, identifier.to);

        // Retrieve the subject's permissions on the object.
        record = etoile::wall::Access::lookup(
          identifier.from, agent::Agent::Subject);

        // Check the record.
        if (!((record != nucleus::neutron::Record::Null) &&
              ((record.permissions & nucleus::neutron::PermissionWrite) ==
               nucleus::neutron::PermissionWrite)))
          error("the subject does not have the right to rename this "
                "directory entry",
                -EACCES,
                identifier.object, identifier.to, identifier.from);

        // Lookup for the target name.
        if (etoile::wall::Directory::Lookup(identifier.to,
                                            t,
                                            entry) == elle::Status::Error)
          error("unable to lookup the target name",
                -EPERM,
                identifier.object, identifier.to, identifier.from);

        // Check if an entry actually exist for the target name
        // meaning that an object is about to get overwritten.
        if (entry != nullptr)
          {
            // In this case, the target object must be destroyed.
            int               result;

            // Unlink the object, assuming it is either a file or a
            // link.  Note that the Crux's method is called in order
            // not to have to deal with the target's genre.
            if ((result = Crux::unlink(target)) < 0)
              error("unable to unlink the target object which is "
                    "about to get overwritte",
                    result,
                    identifier.object, identifier.to, identifier.from);
          }

        // Add an entry.
        if (etoile::wall::Directory::Add(
              identifier.to,
              t,
              identifier.object) == elle::Status::Error)
          error("unable to add an entry to the directory",
                -EPERM,
                identifier.object, identifier.to, identifier.from);

        // Remove the entry.
        if (etoile::wall::Directory::Remove(
              identifier.from,
              f) == elle::Status::Error)
          error("unable to remove a directory entry",
                -EPERM,
                identifier.object, identifier.to, identifier.from);

        // Store the _to_ directory.
        if (etoile::wall::Directory::Store(
              identifier.to) == elle::Status::Error)
          error("unable to store the directory",
                -EPERM,
                identifier.object, identifier.from);

        // Store the _from_ directory.
        if (etoile::wall::Directory::Store(
              identifier.from) == elle::Status::Error)
          error("unable to store the directory",
                -EPERM,
                identifier.object);

        // Store the object.
        etoile::wall::Object::Store(identifier.object);
      }

    // Rename the path associated with the handle in the
    // Crib should the handle reference a freshly created
    // object, hence lying in the Crib.
    if (Crib::Exist(source) == elle::Status::True)
      Crib::rename(source, target);

    return (0);
  }

  /// This method removes an existing file.
  int
  Crux::unlink(const char* path)
  {
    ELLE_TRACE_SCOPE("%s(%s)", __FUNCTION__, path);

    etoile::path::Slab                name;
    etoile::path::Way                 child(path);
    etoile::path::Way                 parent(child, name);
    struct
    {
      etoile::path::Chemin            child;
      etoile::path::Chemin            parent;
    }                                 chemin;
    etoile::gear::Identifier          directory;
    nucleus::neutron::Subject subject;

    // Resolve the path.
    chemin.child = etoile::wall::Path::resolve(child);

    // Load the object.
    etoile::gear::Identifier identifier(
      etoile::wall::Object::Load(chemin.child));

    // Retrieve information on the object.
    etoile::abstract::Object abstract(etoile::wall::Object::Information(identifier));

    // Create a temporary subject based on the object owner's key.
    if (subject.Create(abstract.keys.owner) == elle::Status::Error)
      error("unable to create a temporary subject",
            -EPERM,
            identifier);

    // Check that the subject is the owner of the object.
    if (agent::Agent::Subject != subject)
      error("the subject does not have the right to destroy this object",
            -EACCES,
            identifier);

    // Resolve the path.
    chemin.parent = etoile::wall::Path::resolve(parent);

    // Load the directory.
    if (etoile::wall::Directory::Load(chemin.parent,
                                      directory) == elle::Status::Error)
      error("unable to load the directory",
            -ENOENT,
            identifier);

    // Retrieve the subject's permissions on the object.
    nucleus::neutron::Record record(
      etoile::wall::Access::lookup(directory, agent::Agent::Subject));

    // Check the record.
    if (!((record != nucleus::neutron::Record::Null) &&
          ((record.permissions & nucleus::neutron::PermissionWrite) ==
           nucleus::neutron::PermissionWrite)))
      error("the subject does not have the right to remove an entry from "
            "this directory",
            -EACCES,
            identifier, directory);

    // Remove the object according to its type: file or link.
    switch (abstract.genre)
      {
      case nucleus::neutron::GenreFile:
        {
          // Destroy the file.
          if (etoile::wall::File::Destroy(identifier) == elle::Status::Error)
            error("unable to destroy the file",
                  -EPERM,
                  directory);

          break;
        }
      case nucleus::neutron::GenreLink:
        {
          // Destroy the link.
          if (etoile::wall::Link::Destroy(identifier) == elle::Status::Error)
            error("unable to destroy the link",
                  -EPERM,
                  directory);

          break;
        }
      case nucleus::neutron::GenreDirectory:
        {
          error("meaningless operation: unlink on a directory object",
                -EPERM);
        }
      };

    // Remove the entry.
    if (etoile::wall::Directory::Remove(directory,
                                        name) == elle::Status::Error)
      error("unable to remove a directory entry",
            -EPERM,
            directory);

    // Store the directory.
    if (etoile::wall::Directory::Store(directory) == elle::Status::Error)
      error("unable to store the directory",
            -EPERM);

    return (0);
  }
}
