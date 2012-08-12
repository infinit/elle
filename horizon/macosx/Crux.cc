#include <horizon/macosx/Crux.hh>
#include <horizon/macosx/MacOSX.hh>
#include <horizon/macosx/Janitor.hh>
#include <horizon/macosx/Handle.hh>
#include <horizon/macosx/Crib.hh>

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
#include <etoile/miscellaneous/Abstract.hh>

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

namespace horizon
{
  namespace macosx
  {

//
// ---------- macro-functions -------------------------------------------------
//

    ///
    /// this macro-function makes it easier to return an error from an
    /// upcall, taking care to log the error but also to release the
    /// remaining identifiers.
    ///
#define error(_text_, _errno_, _identifiers_...)                        \
  do                                                                    \
    {                                                                   \
      log(_text_);                                                      \
                                                                        \
      Janitor::Clear(_identifiers_);                                    \
                                                                        \
      return ((_errno_));                                               \
    } while (false)

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant defines the number of directory entries to fetch from
    /// etoile when performing a Readdir().
    ///
    const nucleus::neutron::Size Crux::Range = 128;

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method returns general information on the file system.
    ///
    int                 Crux::Statfs(const char*,
                                     struct ::statvfs*          statvfs)
    {
      // XXX
      statvfs->f_bsize = 4096;
      statvfs->f_frsize = 4096;

      statvfs->f_blocks = 6000000;
      statvfs->f_bfree = 2000000;
      statvfs->f_bavail = statvfs->f_blocks - statvfs->f_bfree;

      statvfs->f_files = 6000000;
      statvfs->f_ffree = 2000000;
      statvfs->f_favail = statvfs->f_files - statvfs->f_ffree;

      statvfs->f_fsid = 0;

      statvfs->f_flag = ST_NOSUID;

      statvfs->f_namemax = 2048;
      // XXX

      return (0);
    }

    ///
    /// this method returns general-purpose information on the file system
    /// object identified by _path_.
    ///
    int                 Crux::Getattr(const char*               path,
                                      struct ::stat*            stat)
    {
      etoile::gear::Identifier  identifier;
      etoile::path::Way         way(path);
      etoile::path::Chemin      chemin;
      struct ::fuse_file_info   info;
      int                       result;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, stat);

      // resolve the path.
      if (etoile::wall::Path::Resolve(way, chemin) == elle::Status::Error)
        {
          // purge the error messages since it may be normal not to be able
          // to resolve the given way.
          purge();

          return (-ENOENT);
        }

      // load the object.
      if (etoile::wall::Object::Load(chemin, identifier) == elle::Status::Error)
        error("unable to load the object",
              -ENOENT);

      // create a local handle.
      Handle                    handle(Handle::OperationGetattr,
                                       identifier);

      // set the handle in the fuse_file_info structure.
      //
      // be careful, the address is local but it is alright since it is
      // used in Fgetattr() only.
      info.fh = reinterpret_cast<uint64_t>(&handle);

      // call the Fgetattr() method.
      if ((result = Crux::Fgetattr(path, stat, &info)) < 0)
        error("unable to get information on the given file descriptor",
              result,
              identifier);

      // discard the object.
      if (etoile::wall::Object::Discard(identifier) == elle::Status::Error)
        error("unable to discard the object",
              -EPERM);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, stat);

      return (0);
    }

    ///
    /// this method returns general-purpose information on the file system
    /// object identified by _path_.
    ///
    int                 Crux::Fgetattr(const char*              path,
                                       struct ::stat*           stat,
                                       struct ::fuse_file_info* info)
    {
      Handle*                           handle;
      etoile::miscellaneous::Abstract   abstract;
      elle::String*                     name;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, stat);

      // clear the stat structure.
      ::memset(stat, 0x0, sizeof (struct ::stat));

      // retrieve the handle.
      handle = reinterpret_cast<Handle*>(info->fh);

      // retrieve information on the object.
      if (etoile::wall::Object::Information(handle->identifier,
                                            abstract) == elle::Status::Error)
        error("unable to retrieve information on the object",
              -EPERM);

      // set the uid by first looking into the users map. if no local user is
      // found, the 'somebody' user is used instead, indicating that the
      // file belongs to someone, with the given permissions, but cannot
      // be mapped to a local user name.
      if (MacOSX::Dictionary.users.Lookup(abstract.keys.owner,
                                          name) == elle::Status::True)
        {
          //
          // in this case, the object's owner is known locally.
          //
          struct ::passwd*      passwd;

          // retrieve the passwd structure associated with this name.
          if ((passwd = ::getpwnam(name->c_str())) != nullptr)
            {
              // set the uid to the local user's.
              stat->st_uid = passwd->pw_uid;
            }
          else
            {
              // if an error occured, set the user to 'somebody'.
              stat->st_uid = MacOSX::Somebody::UID;
            }
        }
      else
        {
          //
          // otherwise, this user is unknown locally, so indicate the
          // system that this object belongs to the 'somebody' user.
          //

          stat->st_uid = MacOSX::Somebody::UID;
        }

      // since Infinit does not have the concept of current group, the
      // group of this object is set to 'somebody'.
      stat->st_gid = MacOSX::Somebody::GID;

      // set the size.
      stat->st_size = static_cast<off_t>(abstract.size);

      // set the disk usage by assuming the smallest disk unit is
      // 512 bytes.
      //
      // note however the the optimised size of I/Os is set to 4096.
      stat->st_blksize = 4096;
      stat->st_blocks =
        (stat->st_size / 512) +
        (stat->st_size % 512) > 0 ? 1 : 0;

      // set the number of hard links to 1 since no hard link exist
      // but the original object.
      stat->st_nlink = 1;

      // convert the times into time_t structures.
      stat->st_atime = time(nullptr);

      if (abstract.stamps.creation.Get(stat->st_ctime) ==
          elle::Status::Error)
        error("unable to convert the time stamps",
              -EPERM);

      if (abstract.stamps.modification.Get(stat->st_mtime) ==
          elle::Status::Error)
        error("unable to convert the time stamps",
              -EPERM);

      // set the mode and permissions.
      switch (abstract.genre)
        {
        case nucleus::neutron::GenreDirectory:
          {
            // set the object as being a directory.
            stat->st_mode = S_IFDIR;

            // if the user has the read permission, allow her to access
            // and read the directory.
            if ((abstract.permissions.owner &
                 nucleus::neutron::PermissionRead) ==
                nucleus::neutron::PermissionRead)
              stat->st_mode |= S_IRUSR | S_IXUSR;

            // if the user has the write permission, allow her to modify
            // the directory content.
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

            // if the user has the read permission, allow her to read
            // the file.
            if ((abstract.permissions.owner &
                 nucleus::neutron::PermissionRead) ==
                nucleus::neutron::PermissionRead)
              stat->st_mode |= S_IRUSR;

            // if the user has the write permission, allow her to modify
            // the file content.
            if ((abstract.permissions.owner &
                 nucleus::neutron::PermissionWrite) ==
                nucleus::neutron::PermissionWrite)
              stat->st_mode |= S_IWUSR;

            // retrieve the attribute.
            if (etoile::wall::Attributes::Get(handle->identifier,
                                              "perm::exec",
                                              trait) == elle::Status::Error)
              error("unable to retrieve an attribute",
                    -EPERM);

            // check the trait.
            if ((trait != nullptr) &&
                (trait->value == "true"))
              {
                // active the exec bit.
                stat->st_mode |= S_IXUSR;
              }

            break;
          }
        case nucleus::neutron::GenreLink:
          {
            stat->st_mode = S_IFLNK;

            // if the user has the read permission, allow her to read and
            // search the linked object.
            if ((abstract.permissions.owner &
                 nucleus::neutron::PermissionRead) ==
                nucleus::neutron::PermissionRead)
              stat->st_mode |= S_IRUSR | S_IXUSR;

            // if the user has the write permission, allow her to modify
            // the link.
            if ((abstract.permissions.owner &
                 nucleus::neutron::PermissionWrite) ==
                nucleus::neutron::PermissionWrite)
              stat->st_mode |= S_IWUSR;

            break;
          }
        default:
          {
            error("unknown genre",
                  -EPERM);
          }
        }

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, stat);

      return (0);
    }

    ///
    /// this method changes the access and modification time of the object.
    ///
    int                 Crux::Utimens(const char*               path,
                                      const struct ::timespec[2])
    {
      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, ...)\n",
               __FUNCTION__,
               path);

      //
      // XXX not supported: do something about it
      //

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, ...)\n",
               __FUNCTION__,
               path);

      return (0);
    }

    ///
    /// this method opens the directory _path_.
    ///
    int                 Crux::Opendir(const char*               path,
                                      struct ::fuse_file_info*  info)
    {
      etoile::gear::Identifier  identifier;
      etoile::path::Way         way(path);
      etoile::path::Chemin      chemin;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, info);

      // resolve the path.
      if (etoile::wall::Path::Resolve(way, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the directory.
      if (etoile::wall::Directory::Load(chemin,
                                        identifier) == elle::Status::Error)
        error("unable to load the directory",
              -ENOENT);

      // duplicate the identifier and save it in the info structure's file
      // handle.
      info->fh =
        reinterpret_cast<uint64_t>(new Handle(Handle::OperationOpendir,
                                              identifier));

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, info);

      return (0);
    }

    ///
    /// this method reads the directory entries.
    ///
    int                 Crux::Readdir(const char*               path,
                                      void*                     buffer,
                                      ::fuse_fill_dir_t         filler,
                                      off_t                     offset,
                                      struct ::fuse_file_info*  info)
    {
      Handle*           handle;
      off_t             next;
      nucleus::neutron::Record const* record;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %p, %p, %qu, %p)\n",
               __FUNCTION__,
               path, buffer, filler,
               static_cast<elle::Natural64>(offset), info);

      // set the handle pointer to the file handle that has been filled by
      // Opendir().
      handle = reinterpret_cast<Handle*>(info->fh);

      // retrieve the subject's permissions on the object.
      if (etoile::wall::Access::Lookup(handle->identifier,
                                       agent::Agent::Subject,
                                       record) == elle::Status::Error)
        error("unable to retrieve the access record",
              -EPERM);

      // check the record.
      if (!((record != nullptr) &&
            ((record->permissions &
              nucleus::neutron::PermissionRead) ==
             nucleus::neutron::PermissionRead)))
        error("the subject does not have the right to read the "
              "directory entries",
              -EACCES);

      // fill the . and .. entries.
      if (offset == 0)
        filler(buffer, ".", nullptr, 1);
      if (offset <= 1)
        filler(buffer, "..", nullptr, 2);

      // compute the offset of the next entry.
      if (offset < 2)
        next = 3;
      else
        next = offset + 1;

      // adjust the offset since Etoile starts with zero while in POSIX
      // terms, zero and one are used for '.' and '..'.
      if (offset > 2)
        offset -= 2;

      while (true)
        {
          nucleus::neutron::Range<nucleus::neutron::Entry> range;
          nucleus::neutron::Range<nucleus::neutron::Entry>::Scoutor scoutor;

          // read the directory entries.
          if (etoile::wall::Directory::Consult(
                handle->identifier,
                static_cast<nucleus::neutron::Index>(offset),
                Crux::Range,
                range) == elle::Status::Error)
            error("unable to retrieve some directory entries",
                  -EPERM);

          // add the entries by using the filler() function.
          for (scoutor = range.container.begin();
               scoutor != range.container.end();
               scoutor++)
            {
              nucleus::neutron::Entry* entry = *scoutor;

              // fill the buffer with filler().
              if (filler(buffer, entry->name.c_str(), nullptr, next) == 1)
                {
                  // debug.
                  if (Infinit::Configuration.horizon.debug == true)
                    printf("[horizon] /Crux::%s(%s, %p, %p, %qu, %p)\n",
                           __FUNCTION__,
                           path, buffer, filler,
                           static_cast<elle::Natural64>(offset), info);

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
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %p, %p, %qu, %p)\n",
               __FUNCTION__,
               path, buffer, filler,
               static_cast<elle::Natural64>(offset), info);

      return (0);
    }

    ///
    /// this method closes the directory _path_.
    ///
    int                 Crux::Releasedir(const char*            path,
                                         struct ::fuse_file_info* info)
    {
      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, info);

      // set the handle pointer to the file handle that has been filled by
      // Opendir().
      std::unique_ptr<Handle> handle(reinterpret_cast<Handle*>(info->fh));

      // discard the object.
      if (etoile::wall::Directory::Discard(
            handle->identifier) == elle::Status::Error)
        error("unable to discard the directory",
              -EPERM);

      // reset the file handle, just to make sure it is not used anymore.
      info->fh = 0;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, info);

      return (0);
    }

    ///
    /// this method creates a directory.
    ///
    int                 Crux::Mkdir(const char*                 path,
                                    mode_t                      mode)
    {
      nucleus::neutron::Permissions permissions =
        nucleus::neutron::PermissionNone;
      etoile::path::Slab        name;
      etoile::path::Way         way(etoile::path::Way(path), name);
      etoile::path::Chemin      chemin;
      etoile::gear::Identifier  directory;
      etoile::gear::Identifier  subdirectory;
      nucleus::neutron::Record const* record;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mode);

      // resolve the path.
      if (etoile::wall::Path::Resolve(way, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the directory.
      if (etoile::wall::Directory::Load(chemin,
                                        directory) == elle::Status::Error)
        error("unable to load the directory",
              -ENOENT);

      // retrieve the subject's permissions on the object.
      if (etoile::wall::Access::Lookup(directory,
                                       agent::Agent::Subject,
                                       record) == elle::Status::Error)
        error("unable to retrieve the access record",
              -EPERM,
              directory);

      // check the record.
      if (!((record != nullptr) &&
            ((record->permissions &
              nucleus::neutron::PermissionWrite) ==
             nucleus::neutron::PermissionWrite)))
        error("the subject does not have the right to create a "
              "subdirectory in this directory",
              -EACCES,
              directory);

      // create the subdirectory.
      if (etoile::wall::Directory::Create(subdirectory) == elle::Status::Error)
        error("unable to create the directory",
              -EPERM,
              directory);

      // compute the permissions.
      if (mode & S_IRUSR)
        permissions |= nucleus::neutron::PermissionRead;

      if (mode & S_IWUSR)
        permissions |= nucleus::neutron::PermissionWrite;

      // set the owner permissions.
      if (etoile::wall::Access::Grant(subdirectory,
                                      agent::Agent::Subject,
                                      permissions) == elle::Status::Error)
        error("unable to update the access record",
              -EPERM,
              subdirectory, directory);

      // add the subdirectory.
      if (etoile::wall::Directory::Add(directory,
                                       name,
                                       subdirectory) == elle::Status::Error)
        error("unable to add an entry to the parent directory",
              -EPERM,
              subdirectory, directory);

      // store the subdirectory.
      if (etoile::wall::Directory::Store(subdirectory) == elle::Status::Error)
        error("unable to store the directory",
              -EPERM,
              directory);

      // store the directory.
      if (etoile::wall::Directory::Store(directory) == elle::Status::Error)
        error("unable to store the directory",
              -EPERM);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mode);

      return (0);
    }

    ///
    /// this method removes a directory.
    ///
    int                 Crux::Rmdir(const char*                 path)
    {
      etoile::path::Slab                name;
      etoile::path::Way                 child(path);
      etoile::path::Way                 parent(child, name);
      etoile::path::Chemin              chemin;
      etoile::gear::Identifier          directory;
      etoile::gear::Identifier          subdirectory;
      etoile::miscellaneous::Abstract   abstract;
      nucleus::neutron::Record const* record;
      nucleus::neutron::Subject subject;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s)\n",
               __FUNCTION__,
               path);

      // resolve the path.
      if (etoile::wall::Path::Resolve(parent, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the directory.
      if (etoile::wall::Directory::Load(chemin,
                                        directory) == elle::Status::Error)
        error("unable to load the directory",
              -ENOENT);

      // retrieve the subject's permissions on the object.
      if (etoile::wall::Access::Lookup(directory,
                                       agent::Agent::Subject,
                                       record) == elle::Status::Error)
        error("unable to retrieve the access record",
              -EPERM,
              directory);

      // check the record.
      if (!((record != nullptr) &&
            ((record->permissions &
              nucleus::neutron::PermissionWrite) ==
             nucleus::neutron::PermissionWrite)))
        error("the subject does not have the right to remove "
              "a subdirectory from this directory",
              -EACCES,
              directory);

      // resolve the path.
      if (etoile::wall::Path::Resolve(child, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT,
              directory);

      // load the subdirectory.
      if (etoile::wall::Directory::Load(chemin,
                                        subdirectory) == elle::Status::Error)
        error("unable to load the directory",
              -ENOENT,
              directory);

      // retrieve information on the object.
      if (etoile::wall::Object::Information(subdirectory,
                                            abstract) == elle::Status::Error)
        error("unable to retrieve information on the object",
              -EPERM,
              subdirectory, directory);

      // create a temporary subject based on the object owner's key.
      if (subject.Create(abstract.keys.owner) == elle::Status::Error)
        error("unable to create a temporary subject",
              -EPERM,
              subdirectory, directory);

      // check that the subject is the owner of the object.
      if (agent::Agent::Subject != subject)
        error("the subject does not have the right to destroy "
              "this directory",
              -EACCES,
              subdirectory, directory);

      // remove the entry.
      if (etoile::wall::Directory::Remove(directory,
                                          name) == elle::Status::Error)
        error("unable to remove a directory entry",
              -EPERM,
              subdirectory, directory);

      // store the directory.
      if (etoile::wall::Directory::Store(directory) == elle::Status::Error)
        error("unable to store the directory",
              -EPERM,
              subdirectory);

      // destroy the subdirectory.
      if (etoile::wall::Directory::Destroy(subdirectory) == elle::Status::Error)
        error("unable to destroy the directory",
              -EPERM);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s)\n",
               __FUNCTION__,
               path);

      return (0);
    }

    ///
    /// this method checks if the current user has the permission to access
    /// the object _path_ for the operations _mask_.
    ///
    int                 Crux::Access(const char*                path,
                                     int                        mask)
    {
      etoile::gear::Identifier          identifier;
      etoile::miscellaneous::Abstract   abstract;
      etoile::path::Way                 way(path);
      etoile::path::Chemin              chemin;
      nucleus::neutron::Record const* record;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mask);

      // resolve the path.
      if (etoile::wall::Path::Resolve(way, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the object.
      if (etoile::wall::Object::Load(chemin, identifier) == elle::Status::Error)
        error("unable to load the object",
              -ENOENT);

      // retrieve information on the object.
      if (etoile::wall::Object::Information(identifier,
                                            abstract) == elle::Status::Error)
        error("unable to retrieve information on the object",
              -EPERM,
              identifier);

      // retrieve the user's permissions on the object.
      if (etoile::wall::Access::Lookup(identifier,
                                       agent::Agent::Subject,
                                       record) == elle::Status::Error)
        error("unable to retrieve the access record",
              -EPERM,
              identifier);

      // check the record.
      if (record == nullptr)
        goto _access;

      // check if the permissions match the mask for execution.
      if (mask & X_OK)
        {
          switch (abstract.genre)
            {
            case nucleus::neutron::GenreDirectory:
              {
                // check if the user has the read permission meaning the
                // exec bit
                if ((record->permissions &
                     nucleus::neutron::PermissionRead) !=
                    nucleus::neutron::PermissionRead)
                  goto _access;

                break;
              }
            case nucleus::neutron::GenreFile:
              {
                nucleus::neutron::Trait const* trait;

                // get the perm::exec attribute
                if (etoile::wall::Attributes::Get(identifier,
                                                  "perm::exec",
                                                  trait) == elle::Status::Error)
                  error("unable to retrieve the attribute",
                        -EPERM,
                        identifier);

                // check the trait.
                if (!((trait != nullptr) &&
                      (trait->value == "true")))
                  goto _access;

                break;
              }
            case nucleus::neutron::GenreLink:
              {
                nucleus::neutron::Trait const* trait;

                // get the perm::exec attribute
                if (etoile::wall::Attributes::Get(identifier,
                                                  "perm::exec",
                                                  trait) == elle::Status::Error)
                  error("unable ti retrive the attribute",
                        -EPERM,
                        identifier);

                // check the trait.
                if (!((trait != nullptr) &&
                      (trait->value == "true")))
                  goto _access;

                break;
              }
            }
        }

      // check if the permissions match the mask for reading.
      if (mask & R_OK)
        {
          if ((record->permissions &
               nucleus::neutron::PermissionRead) !=
              nucleus::neutron::PermissionRead)
            goto _access;
        }

      // check if the permissions match the mask for writing.
      if (mask & W_OK)
        {
          if ((record->permissions &
               nucleus::neutron::PermissionWrite) !=
              nucleus::neutron::PermissionWrite)
            goto _access;
        }

      // discard the object.
      if (etoile::wall::Object::Discard(identifier) == elle::Status::Error)
        error("unable to discard the object",
              -EPERM);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mask);

      return (0);

    _access:
      //
      // at this point, the access has been refused.
      //
      // therefore, the identifier must be discarded while EACCES must
      // be returned.
      //

      // discard the identifier.
      etoile::wall::Object::Discard(identifier);

      // purge the errors.
      purge();

      // return EACCES.
      return (-EACCES);
    }

    ///
    /// this method modifies the permissions on the object.
    ///
    int                 Crux::Chmod(const char*                 path,
                                    mode_t                      mode)
    {
      nucleus::neutron::Permissions permissions = nucleus::neutron::PermissionNone;
      etoile::gear::Identifier          identifier;
      etoile::path::Way                 way(path);
      etoile::path::Chemin              chemin;
      etoile::miscellaneous::Abstract   abstract;
      nucleus::neutron::Subject subject;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, 0%o)\n",
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
      // indeed, on most MACOS X systems, the umask is set to 022 or is
      // somewhat equivalent, granting permissions, by default, to the
      // default group and the others.
      //
      // although this is, from our point of view, a very bad idea, it would
      // be catastrophic to create such access records in Infinit, especially
      // because Infinit has been designed and optimised for objects
      // accessed by their sole owners.
      //

      // compute the permissions.
      if (mode & S_IRUSR)
        permissions |= nucleus::neutron::PermissionRead;

      if (mode & S_IWUSR)
        permissions |= nucleus::neutron::PermissionWrite;

      // resolve the path.
      if (etoile::wall::Path::Resolve(way, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the object.
      if (etoile::wall::Object::Load(chemin, identifier) == elle::Status::Error)
        error("unable to load the object",
              -ENOENT);

      // retrieve information on the object.
      if (etoile::wall::Object::Information(identifier,
                                            abstract) == elle::Status::Error)
        error("unable to retrieve information on the object",
              -EPERM,
              identifier);

      // create a temporary subject based on the object owner's key.
      if (subject.Create(abstract.keys.owner) == elle::Status::Error)
        error("unable to create a temporary subject",
              -EPERM,
              identifier);

      // check that the subject is the owner of the object.
      if (agent::Agent::Subject != subject)
        error("the subject does not have the right to modify the "
              "access permissions on this object",
              -EACCES,
              identifier);

      // the permission modification must be performed according to the
      // object state.
      //
      // indeed, if the object has just been created, the permissions assigned
      // at creation will actually be granted when closed.
      //
      // therefore, should a chmod() be requested between a create() and
      // a close(), the change of permissions should be delayed as it is
      // the case for any file being created.
      //
      // the following therefore checks if the path corresponds to a file
      // in creation. if so, the permissions are recorded for future
      // application.
      //
      if (Crib::Exist(elle::String(path)) == elle::Status::True)
        {
          Handle*       handle;

          //
          // retrieve the handle, representing the created file, from
          // the crib.
          //
          // then update the future permissions in the handle so that,
          // when the file gets closed, these permissions get applied.
          //

          if (Crib::Retrieve(elle::String(path), handle) == elle::Status::Error)
            error("unable to retrieve the handle from the crib",
                  -EBADF,
                  identifier);

          handle->permissions = permissions;
        }
      else
        {
          // update the accesses.
          //
          // note that the method assumes that the caller is the object's
          // owner! if not, an error will occur anyway, so why bother
          // checking.
          if (etoile::wall::Access::Grant(identifier,
                                          agent::Agent::Subject,
                                          permissions) == elle::Status::Error)
            error("unable to update the access records",
                  -EPERM,
                  identifier);
        }

      // if the execution bit is to be set...
      if (mode & S_IXUSR)
        {
          // set the perm::exec attribute if necessary i.e depending on the
          // file genre.
          switch (abstract.genre)
            {
            case nucleus::neutron::GenreFile:
              {
                // set the perm::exec attribute
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
                // nothing to do for the other genres.

                break;
              }
            }
        }

      // store the object.
      if (etoile::wall::Object::Store(identifier) == elle::Status::Error)
        error("unable to store the object",
              -EPERM);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mode);

      return (0);
    }

    ///
    /// this method modifies the owner of a given object.
    ///
    int                 Crux::Chown(const char*                 path,
                                    uid_t                       uid,
                                    gid_t                       gid)
    {
      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %u, %u)\n",
               __FUNCTION__,
               path, uid, gid);

      // XXX to implement.

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %u, %u)\n",
               __FUNCTION__,
               path, uid, gid);

      return (0);
    }

#if defined(HAVE_SETXATTR)
    ///
    /// this method sets an extended attribute value.
    ///
    /// note that the flags are ignored!
    ///
    int                 Crux::Setxattr(const char*              path,
                                       const char*              name,
                                       const char*              value,
                                       size_t                   size,
                                       int                      flags,
                                       uint32_t                 position)
    {
      etoile::gear::Identifier          identifier;
      etoile::path::Way                 way(path);
      etoile::path::Chemin              chemin;
      etoile::miscellaneous::Abstract   abstract;
      nucleus::neutron::Subject subject;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %s, %p, %lu, 0x%x, %u)\n",
               __FUNCTION__,
               path, name, value, size, flags, position);

      // resolve the path.
      if (etoile::wall::Path::Resolve(way, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the object.
      if (etoile::wall::Object::Load(chemin, identifier) == elle::Status::Error)
        error("unable to load the object",
              -ENOENT);

      // retrieve information on the object.
      if (etoile::wall::Object::Information(identifier,
                                            abstract) == elle::Status::Error)
        error("unable to retrieve information on the object",
              -EPERM,
              identifier);

      // create a temporary subject based on the object owner's key.
      if (subject.Create(abstract.keys.owner) == elle::Status::Error)
        error("unable to create a temporary subject",
              -EPERM,
              identifier);

      // check that the subject is the owner of the object.
      if (agent::Agent::Subject != subject)
        error("the subject does not have the right to modify the attributes "
              "associated with this object",
              -EACCES,
              identifier);

      // set the attribute.
      if (etoile::wall::Attributes::Set(identifier,
                                        elle::String(name),
                                        elle::String(value, size)) ==
          elle::Status::Error)
        error("unable to set the attribute",
              -EPERM,
              identifier);

      // store the object.
      if (etoile::wall::Object::Store(identifier) == elle::Status::Error)
        error("unable to store the object",
              -EPERM);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %s, %p, %lu, 0x%x, %u)\n",
               __FUNCTION__,
               path, name, value, size, flags, position);

      return (0);
    }

    ///
    /// this method returns the attribute associated with the given object.
    ///
    int                 Crux::Getxattr(const char*              path,
                                       const char*              name,
                                       char*                    value,
                                       size_t                   size,
                                       uint32_t                 position)
    {
      etoile::gear::Identifier  identifier;
      etoile::path::Way         way(path);
      etoile::path::Chemin      chemin;
      nucleus::neutron::Trait const* trait;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %s, %p, %lu, %u)\n",
               __FUNCTION__,
               path, name, value, size, position);

      // resolve the path.
      if (etoile::wall::Path::Resolve(way, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the object.
      if (etoile::wall::Object::Load(chemin, identifier) == elle::Status::Error)
        error("unable to load the object",
              -ENOENT);

      // get the attribute.
      if (etoile::wall::Attributes::Get(identifier,
                                        elle::String(name),
                                        trait) == elle::Status::Error)
        error("unable to retrieve an attribute",
              -EPERM,
              identifier);

      // discard the object.
      if (etoile::wall::Object::Discard(identifier) == elle::Status::Error)
        error("unable to discard the object",
              -EPERM);

      // test if a trait has been found.
      if (trait == nullptr)
        return (-ENOATTR);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %s, %p, %lu, %u)\n",
               __FUNCTION__,
               path, name, value, size, position);

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
    int                 Crux::Listxattr(const char*             path,
                                        char*                   list,
                                        size_t                  size)
    {
      etoile::gear::Identifier                  identifier;
      etoile::path::Way                         way(path);
      etoile::path::Chemin                      chemin;
      nucleus::neutron::Range<nucleus::neutron::Trait> range;
      nucleus::neutron::Range<nucleus::neutron::Trait>::Scoutor scoutor;
      size_t                                    offset;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %p, %lu)\n",
               __FUNCTION__,
               path, list, size);

      // resolve the path.
      if (etoile::wall::Path::Resolve(way, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the object.
      if (etoile::wall::Object::Load(chemin, identifier) == elle::Status::Error)
        error("unable to load the object",
              -ENOENT);

      // fetch the attributes.
      if (etoile::wall::Attributes::Fetch(identifier,
                                          range) == elle::Status::Error)
        error("unable to fetch the attributes",
              -EPERM,
              identifier);

      // discard the object.
      if (etoile::wall::Object::Discard(identifier) == elle::Status::Error)
        error("unable to discard the object",
              -EPERM);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %p, %lu)\n",
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
              nucleus::neutron::Trait* trait = *scoutor;

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
              nucleus::neutron::Trait* trait = *scoutor;

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
    int                 Crux::Removexattr(const char*           path,
                                          const char*           name)
    {
      etoile::gear::Identifier          identifier;
      etoile::path::Way                 way(path);
      etoile::path::Chemin              chemin;
      etoile::miscellaneous::Abstract   abstract;
      nucleus::neutron::Subject subject;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %s)\n",
               __FUNCTION__,
               path, name);

      // resolve the path.
      if (etoile::wall::Path::Resolve(way, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the object.
      if (etoile::wall::Object::Load(chemin, identifier) == elle::Status::Error)
        error("unable to load the object",
              -ENOENT);

      // retrieve information on the object.
      if (etoile::wall::Object::Information(identifier,
                                            abstract) == elle::Status::Error)
        error("unable to retrieve information on the object",
              -EPERM,
              identifier);

      // create a temporary subject based on the object owner's key.
      if (subject.Create(abstract.keys.owner) == elle::Status::Error)
        error("unable to create a temporary subject",
              -EPERM,
              identifier);

      // check that the subject is the owner of the object.
      if (agent::Agent::Subject != subject)
        error("the subject does not have the right to modify the attributes "
              "associated with this object",
              -EACCES,
              identifier);

      // omit the attribute.
      if (etoile::wall::Attributes::Omit(identifier,
                                         elle::String(name)) ==
          elle::Status::Error)
        error("unable to omit the attributes",
              -EPERM,
              identifier);

      // store the object.
      if (etoile::wall::Object::Store(identifier) == elle::Status::Error)
        error("unable to store the object",
              -EPERM);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %s)\n",
               __FUNCTION__,
               path, name);

      return (0);
    }
#endif

    ///
    /// this method creates a symbolic link.
    ///
    int                 Crux::Symlink(const char*               target,
                                      const char*               source)
    {
      etoile::gear::Identifier  directory;
      etoile::gear::Identifier  link;
      etoile::path::Slab        name;
      etoile::path::Way         from(etoile::path::Way(source), name);
      etoile::path::Way         to(target);
      etoile::path::Chemin      chemin;
      nucleus::neutron::Record const* record;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %s)\n",
               __FUNCTION__,
               target, source);

      // resolve the path.
      if (etoile::wall::Path::Resolve(from, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the directory.
      if (etoile::wall::Directory::Load(chemin,
                                        directory) == elle::Status::Error)
        error("unable to load the directory",
              -ENOENT);

      // retrieve the subject's permissions on the object.
      if (etoile::wall::Access::Lookup(directory,
                                       agent::Agent::Subject,
                                       record) == elle::Status::Error)
        error("unable to retrieve the access record",
              -EPERM,
              directory);

      // check the record.
      if (!((record != nullptr) &&
            ((record->permissions &
              nucleus::neutron::PermissionWrite) ==
             nucleus::neutron::PermissionWrite)))
        error("the subject does not have the right to create a link in "
              "this directory",
              -EACCES,
              directory);

      // create a link
      if (etoile::wall::Link::Create(link) == elle::Status::Error)
        error("unable to create a link",
              -EPERM,
              directory);

      // bind the link.
      if (etoile::wall::Link::Bind(link, to) == elle::Status::Error)
        error("unable to bind the link",
              -EPERM,
              link, directory);

      // add an entry for the link.
      if (etoile::wall::Directory::Add(directory,
                                       name,
                                       link) == elle::Status::Error)
        error("unable to add an entry to the directory",
              -EPERM,
              link, directory);

      // store the link.
      if (etoile::wall::Link::Store(link) == elle::Status::Error)
        error("unable to store the link",
              -EPERM,
              directory);

      // store the modified directory.
      if (etoile::wall::Directory::Store(directory) == elle::Status::Error)
        error("unable to store the directory",
              -EPERM);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %s)\n",
               __FUNCTION__,
               target, source);

      return (0);
    }

    ///
    /// this method returns the target path pointed by the symbolic link.
    ///
    int                 Crux::Readlink(const char*              path,
                                       char*                    buffer,
                                       size_t                   size)
    {
      etoile::gear::Identifier  identifier;
      etoile::path::Way         way(path);
      etoile::path::Chemin      chemin;
      etoile::path::Way         target;
      nucleus::neutron::Record const* record;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %p, %qu)\n",
               __FUNCTION__,
               path, buffer, static_cast<elle::Natural64>(size));

      // resolve the path.
      if (etoile::wall::Path::Resolve(way, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the link.
      if (etoile::wall::Link::Load(chemin, identifier) == elle::Status::Error)
        error("unable to load the link",
              -ENOENT);

      // retrieve the subject's permissions on the object.
      if (etoile::wall::Access::Lookup(identifier,
                                       agent::Agent::Subject,
                                       record) == elle::Status::Error)
        error("unable to retrieve the access record",
              -EPERM,
              identifier);

      // check the record.
      if (!((record != nullptr) &&
            ((record->permissions &
              nucleus::neutron::PermissionRead) ==
             nucleus::neutron::PermissionRead)))
        error("the subject does not have the right to read this link",
              -EACCES,
              identifier);

      // resolve the link.
      if (etoile::wall::Link::Resolve(identifier, target) == elle::Status::Error)
        error("unable to resolve the link",
              -EPERM,
              identifier);

      // discard the link.
      if (etoile::wall::Link::Discard(identifier) == elle::Status::Error)
        error("unable to discard the link",
              -EPERM);

      // copy as much as possible of the target into the output buffer.
      ::strncpy(buffer,
                target.path.c_str(),
                (target.path.length() + 1) < size ?
                target.path.length() + 1 :
                size);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %p, %qu)\n",
               __FUNCTION__,
               path, buffer, static_cast<elle::Natural64>(size));

      return (0);
    }

    ///
    /// this method creates a new file and opens it.
    ///
    int                 Crux::Create(const char*                path,
                                     mode_t                     mode,
                                     struct ::fuse_file_info*   info)
    {
      nucleus::neutron::Permissions permissions =
        nucleus::neutron::PermissionNone;
      etoile::path::Slab        name;
      etoile::path::Way         way(etoile::path::Way(path), name);
      etoile::path::Chemin      chemin;
      etoile::gear::Identifier  directory;
      etoile::gear::Identifier  file;
      nucleus::neutron::Record const* record;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, 0%o, %p)\n",
               __FUNCTION__,
               path, mode, info);

      // resolve the path.
      if (etoile::wall::Path::Resolve(way, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the directory.
      if (etoile::wall::Directory::Load(chemin,
                                        directory) == elle::Status::Error)
        error("unable to load the directory",
              -ENOENT);

      // retrieve the subject's permissions on the object.
      if (etoile::wall::Access::Lookup(directory,
                                       agent::Agent::Subject,
                                       record) == elle::Status::Error)
        error("unable to retrieve the access record",
              -EPERM,
              directory);

      // check the record.
      if (!((record != nullptr) &&
            ((record->permissions &
              nucleus::neutron::PermissionWrite) ==
             nucleus::neutron::PermissionWrite)))
        error("the subject does not have the right to create a file in "
              "this directory",
              -EACCES,
              directory);

      // create the file.
      if (etoile::wall::File::Create(file) == elle::Status::Error)
        error("unable to create a file",
              -EPERM,
              directory);

      // set default permissions: read and write.
      permissions =
        nucleus::neutron::PermissionRead | nucleus::neutron::PermissionWrite;

      // set the owner permissions.
      if (etoile::wall::Access::Grant(file,
                                      agent::Agent::Subject,
                                      permissions) == elle::Status::Error)
        error("unable to update the access records",
              -EPERM,
              file, directory);

      // if the file has the exec bit, add the perm::exec attribute.
      if (mode & S_IXUSR)
        {
          // set the perm::exec attribute
          if (etoile::wall::Attributes::Set(file,
                                            "perm::exec",
                                            "true") == elle::Status::Error)
            error("unable to set the attributes",
                  -EPERM,
                  file, directory);
        }

      // add the file to the directory.
      if (etoile::wall::Directory::Add(directory,
                                       name,
                                       file) == elle::Status::Error)
        error("unable to add an entry to the directory",
              -EPERM,
              file, directory);

      // store the file, ensuring the file system consistency.
      //
      // indeed, if the file is kept opened and the directory is stored
      // someone could see an incorrect entry. although errors will occur
      // extremely rarely and such errors do not cause harm, especially
      // considering the Infinit consistency guaranties, we still prefer
      // to do things right, at least for now.
      if (etoile::wall::File::Store(file) == elle::Status::Error)
        error("unable to store the file",
              -EPERM,
              directory);

      // store the directory.
      if (etoile::wall::Directory::Store(directory) == elle::Status::Error)
        error("unable to store the directory",
              -EPERM);

      // resolve the path.
      if (etoile::wall::Path::Resolve(etoile::path::Way(path),
                                      chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // finally, the file is reopened.
      if (etoile::wall::File::Load(chemin, file) == elle::Status::Error)
        error("unable to load the file",
              -ENOENT);

      // compute the future permissions as the current ones are temporary.
      permissions = nucleus::neutron::PermissionNone;

      if (mode & S_IRUSR)
        permissions |= nucleus::neutron::PermissionRead;

      if (mode & S_IWUSR)
        permissions |= nucleus::neutron::PermissionWrite;

      // store the identifier in the file handle.
      info->fh =
        reinterpret_cast<uint64_t>(new Handle(Handle::OperationCreate,
                                              file,
                                              permissions));

      // add the created and opened file in the crib.
      if (Crib::Add(elle::String(path),
                    reinterpret_cast<Handle*>(info->fh)) == elle::Status::Error)
        error("unable to add the created file to the crib",
              -EBADF);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, 0%o, %p)\n",
               __FUNCTION__,
               path, mode, info);

      return (0);
    }

    ///
    /// this method opens a file.
    ///
    int                 Crux::Open(const char*                  path,
                                   struct ::fuse_file_info*     info)
    {
      etoile::path::Way         way(path);
      etoile::path::Chemin      chemin;
      etoile::gear::Identifier  identifier;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, info);

      // resolve the path.
      if (etoile::wall::Path::Resolve(way, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the file.
      if (etoile::wall::File::Load(chemin, identifier) == elle::Status::Error)
        error("unable to load the file",
              -ENOENT);

      // store the identifier in the file handle.
      info->fh =
        reinterpret_cast<uint64_t>(new Handle(Handle::OperationOpen,
                                              identifier));

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, info);

      return (0);
    }

    ///
    /// this method writes data to a file.
    ///
    int                 Crux::Write(const char*                 path,
                                    const char*                 buffer,
                                    size_t                      size,
                                    off_t                       offset,
                                    struct ::fuse_file_info*    info)
    {
      Handle*           handle;
      elle::standalone::Region      region;
      nucleus::neutron::Record const* record;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %p, %qu, %qu, %p)\n",
               __FUNCTION__,
               path, buffer, static_cast<elle::Natural64>(size),
               static_cast<elle::Natural64>(offset), info);

      // retrieve the handle;
      handle = reinterpret_cast<Handle*>(info->fh);

      // retrieve the subject's permissions on the object.
      if (etoile::wall::Access::Lookup(handle->identifier,
                                       agent::Agent::Subject,
                                       record) == elle::Status::Error)
        error("unable to retrieve the access record",
              -EPERM);

      // check the record.
      if (!((record != nullptr) &&
            ((record->permissions & nucleus::neutron::PermissionWrite) ==
             nucleus::neutron::PermissionWrite)))
        error("the subject does not have the right to update this file",
              -EACCES);

      // wrap the buffer.
      if (region.Wrap(reinterpret_cast<const elle::Byte*>(buffer),
                      size) == elle::Status::Error)
        error("unable to wrap the buffer",
              -EPERM);

      // write the file.
      if (etoile::wall::File::Write(
            handle->identifier,
            static_cast<nucleus::neutron::Offset>(offset),
            region) == elle::Status::Error)
        error("unable to write the file",
              -EPERM);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %p, %qu, %qu, %p)\n",
               __FUNCTION__,
               path, buffer, static_cast<elle::Natural64>(size),
               static_cast<elle::Natural64>(offset), info);

      return (size);
    }

    ///
    /// this method reads data from a file.
    ///
    int                 Crux::Read(const char*                  path,
                                   char*                        buffer,
                                   size_t                       size,
                                   off_t                        offset,
                                   struct ::fuse_file_info*     info)
    {
      Handle*           handle;
      elle::standalone::Region      region;
      nucleus::neutron::Record const* record;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %p, %qu, %qu, %p)\n",
               __FUNCTION__,
               path, buffer, static_cast<elle::Natural64>(size),
               static_cast<elle::Natural64>(offset), info);

      // retrieve the handle.
      handle = reinterpret_cast<Handle*>(info->fh);

      // retrieve the subject's permissions on the object.
      if (etoile::wall::Access::Lookup(handle->identifier,
                                       agent::Agent::Subject,
                                       record) == elle::Status::Error)
        error("unable to retrieve the access record",
              -EPERM);

      // check the record.
      if (!((record != nullptr) &&
            ((record->permissions & nucleus::neutron::PermissionRead) ==
             nucleus::neutron::PermissionRead)))
        error("the subject does not have the right to read this file",
              -EACCES);

      // read the file.
      if (etoile::wall::File::Read(
            handle->identifier,
            static_cast<nucleus::neutron::Offset>(offset),
            static_cast<nucleus::neutron::Size>(size),
            region) == elle::Status::Error)
        error("unable to read the file",
              -EPERM);

      // copy the data to the output buffer.
      ::memcpy(buffer, region.contents, region.size);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %p, %qu, %qu, %p)\n",
               __FUNCTION__,
               path, buffer, static_cast<elle::Natural64>(size),
               static_cast<elle::Natural64>(offset), info);

      return (region.size);
    }

    ///
    /// this method modifies the size of a file.
    ///
    int                 Crux::Truncate(const char*              path,
                                       off_t                    size)
    {
      etoile::gear::Identifier  identifier;
      etoile::path::Way         way(path);
      etoile::path::Chemin      chemin;
      struct ::fuse_file_info   info;
      int                       result;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %qu)\n",
               __FUNCTION__,
               path, static_cast<elle::Natural64>(size));

      // resolve the path.
      if (etoile::wall::Path::Resolve(way, chemin) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the file.
      if (etoile::wall::File::Load(chemin, identifier) == elle::Status::Error)
        error("unable to load the file",
              -ENOENT);

      // create a local handle.
      Handle                    handle(Handle::OperationTruncate,
                                       identifier);

      // set the handle in the fuse_file_info structure.
      info.fh = reinterpret_cast<uint64_t>(&handle);

      // call the Ftruncate() method.
      if ((result = Crux::Ftruncate(path, size, &info)) < 0)
        error("unable to truncate the given file descriptpr",
              result,
              identifier);

      // store the file.
      if (etoile::wall::File::Store(identifier) == elle::Status::Error)
        error("unable to store the file",
              -EPERM);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %qu)\n",
               __FUNCTION__,
               path, static_cast<elle::Natural64>(size));

      return (result);
    }

    ///
    /// this method modifies the size of an opened file.
    ///
    int                 Crux::Ftruncate(const char*             path,
                                        off_t                   size,
                                        struct ::fuse_file_info* info)
    {
      Handle*           handle;
      nucleus::neutron::Record const* record;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %qu, %p)\n",
               __FUNCTION__,
               path, static_cast<elle::Natural64>(size), info);

      // retrieve the handle.
      handle = reinterpret_cast<Handle*>(info->fh);

      // retrieve the subject's permissions on the object.
      if (etoile::wall::Access::Lookup(handle->identifier,
                                       agent::Agent::Subject,
                                       record) == elle::Status::Error)
        error("unable to retrieve the access record",
              -EPERM);

      // check the record.
      if (!((record != nullptr) &&
            ((record->permissions & nucleus::neutron::PermissionWrite) ==
             nucleus::neutron::PermissionWrite)))
        error("the subject does not have the right to modify the size of "
              "this file",
              -EACCES);

      // adjust the file's size.
      if (etoile::wall::File::Adjust(handle->identifier,
                                     size) == elle::Status::Error)
        error("unable to adjust the size of the file",
              -EPERM);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %qu, %p)\n",
               __FUNCTION__,
               path, static_cast<elle::Natural64>(size), info);

      return (0);
    }

    ///
    /// this method closes a file.
    ///
    int                 Crux::Release(const char*               path,
                                      struct ::fuse_file_info*  info)
    {
      etoile::path::Way way(path);
      Handle*           handle;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, info);

      // retrieve the handle.
      handle = reinterpret_cast<Handle*>(info->fh);

      // perform final actions depending on the initial operation.
      switch (handle->operation)
        {
        case Handle::OperationCreate:
          {
            // remove the created and opened file in the crib.
            if (Crib::Remove(elle::String(path)) == elle::Status::Error)
              error("unable to remove the created file from the crib",
                    -EBADF,
                    handle->identifier);

            //
            // the permissions settings have been delayed in order to support
            // a read-only file being copied in which case a file is created
            // with read-only permissions before being written.
            //
            // such a normal behaviour would result in runtime permission
            // errors. therefore, the permissions are set once the created
            // file is released in order to overcome this issue.
            //

            // set the owner permissions.
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
            //
            // nothing special to do.
            //

            break;
          }
        }

      // store the file.
      if (etoile::wall::File::Store(handle->identifier) == elle::Status::Error)
        error("unable to store the file",
              -EPERM);

      // delete the handle.
      delete handle;

      // reset the file handle.
      info->fh = 0;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, info);

      return (0);
    }

    ///
    /// this method renames a file.
    ///
    int                 Crux::Rename(const char*                source,
                                     const char*                target)
    {
      etoile::path::Slab        f;
      etoile::path::Way         from(etoile::path::Way(source), f);
      etoile::path::Slab        t;
      etoile::path::Way         to(etoile::path::Way(target), t);
      etoile::gear::Identifier  object;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s, %s)\n",
               __FUNCTION__,
               source, target);

      // if the source and target directories are identical.
      if (from == to)
        {
          //
          // in this case, the object to move can simply be renamed since
          // the source and target directory are identical.
          //
          etoile::path::Chemin          chemin;
          etoile::gear::Identifier      directory;
          nucleus::neutron::Entry const* entry;
          nucleus::neutron::Record const* record;

          // resolve the path.
          if (etoile::wall::Path::Resolve(from, chemin) == elle::Status::Error)
            error("unable to resolve the path",
                  -ENOENT);

          // load the directory.
          if (etoile::wall::Directory::Load(chemin,
                                            directory) == elle::Status::Error)
            error("unable to load the directory",
                  -ENOENT);

          // retrieve the subject's permissions on the object.
          if (etoile::wall::Access::Lookup(directory,
                                           agent::Agent::Subject,
                                           record) == elle::Status::Error)
            error("unable to retrieve the access record",
                  -EPERM,
                  directory);

          // check the record.
          if (!((record != nullptr) &&
                ((record->permissions & nucleus::neutron::PermissionWrite) ==
                 nucleus::neutron::PermissionWrite)))
            error("the subject does not have the right to rename this "
                  "directory entry",
                  -EACCES,
                  directory);

          // lookup for the target name.
          if (etoile::wall::Directory::Lookup(directory,
                                              t,
                                              entry) == elle::Status::Error)
            error("unable to lookup the target name",
                  -EPERM,
                  directory);

          // check if an entry actually exist for the target name meaning
          // that an object is about to get overwritten.
          if (entry != nullptr)
            {
              //
              // in this case, the target object must be destroyed.
              //
              int               result;

              // unlink the object, assuming it is either a file or a link.
              //
              // note that the Crux's method is called in order not to have
              // to deal with the target's genre.
              if ((result = Crux::Unlink(target)) < 0)
                error("unable to unlink the target object which is "
                      "about to get overwritte",
                      result,
                      directory);
            }

          // rename the entry from _f_ to _t_.
          if (etoile::wall::Directory::Rename(directory,
                                              f,
                                              t) == elle::Status::Error)
            error("unable to rename a directory entry",
                  -EPERM,
                  directory);

          // store the directory.
          if (etoile::wall::Directory::Store(directory) == elle::Status::Error)
            error("unable to store the directory",
                  -EPERM);
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
          etoile::path::Way             way(source);
          etoile::path::Chemin          chemin;
          struct
          {
            etoile::gear::Identifier    object;
            etoile::gear::Identifier    from;
            etoile::gear::Identifier    to;
          }                             identifier;
          nucleus::neutron::Entry const* entry;
          nucleus::neutron::Record const* record;

          // resolve the path.
          if (etoile::wall::Path::Resolve(way, chemin) == elle::Status::Error)
            error("unable to resolve the path",
                  -ENOENT);

          // load the object even though we don't know its genre as we
          // do not need to know to perform this operation.
          if (etoile::wall::Object::Load(
                chemin,
                identifier.object) == elle::Status::Error)
            error("unable to load the object",
                  -ENOENT);

          // resolve the path.
          if (etoile::wall::Path::Resolve(to, chemin) == elle::Status::Error)
            error("unable to resolve the path",
                  -ENOENT,
                  identifier.object);

          // load the _to_ directory.
          if (etoile::wall::Directory::Load(
                chemin,
                identifier.to) == elle::Status::Error)
            error("unable to load the directory",
                  -ENOENT,
                  identifier.object);

          // retrieve the subject's permissions on the object.
          if (etoile::wall::Access::Lookup(identifier.to,
                                           agent::Agent::Subject,
                                           record) == elle::Status::Error)
            error("unable to retrieve the access record",
                  -EPERM,
                  identifier.object, identifier.to);

          // check the record.
          if (!((record != nullptr) &&
                ((record->permissions & nucleus::neutron::PermissionWrite) ==
                 nucleus::neutron::PermissionWrite)))
            error("the subject does not have the right to rename this "
                  "directory entry",
                  -EACCES,
                  identifier.object, identifier.to);

          // resolve the path.
          if (etoile::wall::Path::Resolve(from, chemin) == elle::Status::Error)
            error("unable to resolve the path",
                  -ENOENT,
                  identifier.object, identifier.to);

          // load the _from_ directory.
          if (etoile::wall::Directory::Load(
                chemin,
                identifier.from) == elle::Status::Error)
            error("unable to load the directory",
                  -ENOENT,
                  identifier.object, identifier.to);

          // retrieve the subject's permissions on the object.
          if (etoile::wall::Access::Lookup(identifier.from,
                                           agent::Agent::Subject,
                                           record) == elle::Status::Error)
            error("unable to retrieve the access record",
                  -EPERM,
                  identifier.object, identifier.to, identifier.from);

          // check the record.
          if (!((record != nullptr) &&
                ((record->permissions & nucleus::neutron::PermissionWrite) ==
                 nucleus::neutron::PermissionWrite)))
            error("the subject does not have the right to rename this "
                  "directory entry",
                  -EACCES,
                  identifier.object, identifier.to, identifier.from);

          // lookup for the target name.
          if (etoile::wall::Directory::Lookup(identifier.to,
                                              t,
                                              entry) == elle::Status::Error)
            error("unable to lookup the target name",
                  -EPERM,
                  identifier.object, identifier.to, identifier.from);

          // check if an entry actually exist for the target name meaning
          // that an object is about to get overwritten.
          if (entry != nullptr)
            {
              //
              // in this case, the target object must be destroyed.
              //
              int               result;

              // unlink the object, assuming it is either a file or a link.
              //
              // note that the Crux's method is called in order not to have
              // to deal with the target's genre.
              if ((result = Crux::Unlink(target)) < 0)
                error("unable to unlink the target object which is "
                      "about to get overwritte",
                      result,
                      identifier.object, identifier.to, identifier.from);
            }

          // add an entry.
          if (etoile::wall::Directory::Add(
                identifier.to,
                t,
                identifier.object) == elle::Status::Error)
            error("unable to add an entry to the directory",
                  -EPERM,
                  identifier.object, identifier.to, identifier.from);

          // remove the entry.
          if (etoile::wall::Directory::Remove(
                identifier.from,
                f) == elle::Status::Error)
            error("unable to remove a directory entry",
                  -EPERM,
                  identifier.object, identifier.to, identifier.from);

          // store the _to_ directory.
          if (etoile::wall::Directory::Store(
                identifier.to) == elle::Status::Error)
            error("unable to store the directory",
                  -EPERM,
                  identifier.object, identifier.from);

          // store the _from_ directory.
          if (etoile::wall::Directory::Store(
                identifier.from) == elle::Status::Error)
            error("unable to store the directory",
                  -EPERM,
                  identifier.object);

          // store the object.
          if (etoile::wall::Object::Store(
                identifier.object) == elle::Status::Error)
            error("unable to store the object",
                  -EPERM);
        }

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s, %s)\n",
               __FUNCTION__,
               source, target);

      return (0);
    }

    ///
    /// this method removes an existing file.
    ///
    int                 Crux::Unlink(const char*                path)
    {
      etoile::path::Slab                name;
      etoile::path::Way                 child(path);
      etoile::path::Way                 parent(child, name);
      struct
      {
        etoile::path::Chemin            child;
        etoile::path::Chemin            parent;
      }                                 chemin;
      etoile::gear::Identifier          directory;
      etoile::gear::Identifier          identifier;
      etoile::miscellaneous::Abstract   abstract;
      nucleus::neutron::Record const* record;
      nucleus::neutron::Subject subject;

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] Crux::%s(%s)\n",
               __FUNCTION__,
               path);

      // resolve the path.
      if (etoile::wall::Path::Resolve(child,
                                      chemin.child) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT);

      // load the object.
      if (etoile::wall::Object::Load(chemin.child,
                                     identifier) == elle::Status::Error)
        error("unable to load the object",
              -ENOENT);

      // retrieve information on the object.
      if (etoile::wall::Object::Information(identifier,
                                            abstract) == elle::Status::Error)
        error("unable to retrieve information on the object",
              -EPERM,
              identifier);

      // create a temporary subject based on the object owner's key.
      if (subject.Create(abstract.keys.owner) == elle::Status::Error)
        error("unable to create a temporary subject",
              -EPERM,
              identifier);

      // check that the subject is the owner of the object.
      if (agent::Agent::Subject != subject)
        error("the subject does not have the right to destroy this object",
              -EACCES,
              identifier);


      // resolve the path.
      if (etoile::wall::Path::Resolve(parent,
                                      chemin.parent) == elle::Status::Error)
        error("unable to resolve the path",
              -ENOENT,
              identifier);

      // load the directory.
      if (etoile::wall::Directory::Load(chemin.parent,
                                        directory) == elle::Status::Error)
        error("unable to load the directory",
              -ENOENT,
              identifier);

      // retrieve the subject's permissions on the object.
      if (etoile::wall::Access::Lookup(directory,
                                       agent::Agent::Subject,
                                       record) == elle::Status::Error)
        error("unable to retrieve the access record",
              -EPERM,
              identifier, directory);

      // check the record.
      if (!((record != nullptr) &&
            ((record->permissions & nucleus::neutron::PermissionWrite) ==
             nucleus::neutron::PermissionWrite)))
        error("the subject does not have the right to remove an entry from "
              "this directory",
              -EACCES,
              identifier, directory);

      // remove the object according to its type: file or link.
      switch (abstract.genre)
        {
        case nucleus::neutron::GenreFile:
          {
            // destroy the file.
            if (etoile::wall::File::Destroy(identifier) == elle::Status::Error)
              error("unable to destroy the file",
                    -EPERM,
                    directory);

            break;
          }
        case nucleus::neutron::GenreLink:
          {
            // destroy the link.
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

      // remove the entry.
      if (etoile::wall::Directory::Remove(directory,
                                          name) == elle::Status::Error)
        error("unable to remove a directory entry",
              -EPERM,
              directory);

      // store the directory.
      if (etoile::wall::Directory::Store(directory) == elle::Status::Error)
        error("unable to store the directory",
              -EPERM);

      // debug.
      if (Infinit::Configuration.horizon.debug == true)
        printf("[horizon] /Crux::%s(%s)\n",
               __FUNCTION__,
               path);

      return (0);
    }

  }
}
