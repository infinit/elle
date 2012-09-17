#include <horizon/macosx/Crux.hh>
#include <horizon/Crux.hh>

#include <elle/log.hh>

#include <elle/idiom/Close.hh>
# include <sys/statvfs.h>
#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("infinit.horizon.macosx.Crux");

namespace horizon
{
  namespace macosx
  {

//
// ---------- callbacks -------------------------------------------------------
//

    int                 Crux::statfs(const char*,
                                     struct ::statvfs* statvfs)
    {
      ELLE_TRACE_SCOPE("%s()", __FUNCTION__);

      // XXX[these numbers are based on the return value of statvfs() on
      //     a local file system; just to make sure the Finder does not
      //     complain there is not storage space left]
      statvfs->f_bsize = 1048576;
      statvfs->f_frsize = 4096;
      statvfs->f_blocks = 29412370;
      statvfs->f_bfree = 1096215;
      statvfs->f_bavail = 1032215;
      statvfs->f_files = 29412368;
      statvfs->f_ffree = 1032215;
      statvfs->f_favail = 1032215;
      statvfs->f_fsid = 234881026;
      statvfs->f_flag = ST_NOSUID;
      statvfs->f_namemax = 255;
      // XXX

      return (0);
    }

#if defined(HAVE_SETXATTR)
    int
    Crux::setxattr(const char* path,
                   const char* name,
                   const char* value,
                   size_t size,
                   int flags,
                   uint32_t)
    {
<<<<<<< HEAD
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
            ((record->permissions & nucleus::neutron::PermissionWrite) ==
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

      switch (hole::Hole::Descriptor.data().policy())
        {
        case horizon::Policy::accessible:
          {
            // grant the read permission to the 'everybody' group.
            if (etoile::wall::Access::Grant(
                  link,
                  hole::Hole::Descriptor.meta().everybody_subject(),
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
    int                 Crux::readlink(const char*              path,
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
    int                 Crux::create(const char*                path,
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

      switch (hole::Hole::Descriptor.data().policy())
        {
        case horizon::Policy::accessible:
          {
            // grant the read permission to the 'everybody' group.
            if (etoile::wall::Access::Grant(
                  file,
                  hole::Hole::Descriptor.meta().everybody_subject(),
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
    int                 Crux::open(const char*                  path,
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
    int                 Crux::write(const char*                 path,
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
    int                 Crux::read(const char*                  path,
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
    int                 Crux::truncate(const char*              path,
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

      // call the ftruncate() method.
      if ((result = Crux::ftruncate(path, size, &info)) < 0)
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
    int                 Crux::ftruncate(const char*             path,
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
    int                 Crux::release(const char*               path,
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
    int                 Crux::rename(const char*                source,
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
              if ((result = Crux::unlink(target)) < 0)
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
              if ((result = Crux::unlink(target)) < 0)
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
    int                 Crux::unlink(const char*                path)
=======
      return (horizon::Crux::setxattr(path, name, value, size, flags));
    }

    int
    Crux::getxattr(const char* path,
                   const char* name,
                   char* value,
                   size_t size,
                   uint32_t)
>>>>>>> factor-crux
    {
      return (horizon::Crux::getxattr(path, name, value, size));
    }
#endif

  }
}
