//
// ---------- header ----------------------------------------------------------
//
// project       diary
//
// license       infinit
//
// author        julien quintard   [tue jun 28 12:46:13 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <satellites/diary/unix/Crux.hh>

#include <Infinit.hh>

namespace satellite
{
  #undef unix
  namespace unix
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this structure contains the function pointers to the FUSE
    /// implementation routines.
    ///
    struct ::fuse_operations            Crux::Operations;

    ///
    /// this variable contains the path to the target directory which
    /// is made accessible i.e mirrored through the FUSE file system.
    ///
    elle::String                        Crux::Mirror;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// the methods below represents the FUSE upcalls.
    ///

    int                 Crux::Getattr(const char*               path,
                                      struct ::stat*            stbuf)
    {
      elle::String      way(Crux::Mirror + path);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, stat);

      if (::lstat(way.c_str(), stbuf) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %px)\n",
               __FUNCTION__,
               path, stbuf);

      return 0;
    }

    int                 Crux::Fgetattr(const char*              path,
                                       struct ::stat*           stbuf,
                                       struct ::fuse_file_info* fi)
    {
      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, stat);

      (void)path;

      if (::fstat(fi->fh, stbuf) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, stat);

      return 0;
    }

    int                 Crux::Utimens(const char*               path,
                                      const struct ::timespec   ts[2])
    {
      elle::String      way(Crux::Mirror + path);
      struct ::timeval  tv[2];

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, ...)\n",
               __FUNCTION__,
               path);

      tv[0].tv_sec = ts[0].tv_sec;
      tv[0].tv_usec = ts[0].tv_nsec / 1000;
      tv[1].tv_sec = ts[1].tv_sec;
      tv[1].tv_usec = ts[1].tv_nsec / 1000;

      if (::utimes(way.c_str(), tv) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, ...)\n",
               __FUNCTION__,
               path);

      return 0;
    }

    int                 Crux::Opendir(const char*               path,
                                      struct ::fuse_file_info*  fi)
    {
      elle::String      way(Crux::Mirror + path);
      int               res;
      Crux::Handle*     d;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      if ((d =
           static_cast<Crux::Handle*>(::malloc(sizeof(Crux::Handle)))) == nullptr)
        return -ENOMEM;

      if ((d->dp = ::opendir(way.c_str())) == nullptr)
        {
          res = -errno;
          free(d);
          return res;
        }

      d->offset = 0;
      d->entry = nullptr;

      fi->fh = reinterpret_cast<unsigned long>(d);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      return 0;
    }

    int                 Crux::Readdir(const char*               path,
                                      void*                     buf,
                                      ::fuse_fill_dir_t         filler,
                                      off_t                     offset,
                                      struct ::fuse_file_info*  fi)
    {
      Crux::Handle*     d = reinterpret_cast<Crux::Handle*>(fi->fh);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %p, %p, %qu, %p)\n",
               __FUNCTION__,
               path, buf, filler,
               static_cast<elle::Natural64>(offset), fi);

      (void)path;

      if (offset != d->offset)
        {
          ::seekdir(d->dp, offset);

          d->entry = nullptr;
          d->offset = offset;
        }

      while (1)
        {
          struct ::stat st;
          off_t         nextoff;

          if (!d->entry)
            {
              if ((d->entry = ::readdir(d->dp)) == nullptr)
                break;
            }

          ::memset(&st, 0, sizeof(st));

          st.st_ino = d->entry->d_ino;
          st.st_mode = d->entry->d_type << 12;

          nextoff = ::telldir(d->dp);

          if (filler(buf, d->entry->d_name, &st, nextoff))
            break;

          d->entry = nullptr;
          d->offset = nextoff;
        }

      // debug,
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %p, %p, %qu, %p)\n",
               __FUNCTION__,
               path, buf, filler,
               static_cast<elle::Natural64>(offset), fi);

      return 0;
    }

    int                 Crux::Releasedir(const char*            path,
                                         struct ::fuse_file_info* fi)
    {
      Crux::Handle*     d = reinterpret_cast<Crux::Handle*>(fi->fh);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      (void)path;

      ::closedir(d->dp);

      ::free(d);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      return 0;
    }

    int                 Crux::Mkdir(const char*                 path,
                                    mode_t                      mode)
    {
      elle::String      way(Crux::Mirror + path);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mode);

      if (::mkdir(way.c_str(), mode) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mode);

      return 0;
    }

    int                 Crux::Rmdir(const char*                 path)
    {
      elle::String      way(Crux::Mirror + path);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s)\n",
               __FUNCTION__,
               path);

      if (::rmdir(way.c_str()) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s)\n",
               __FUNCTION__,
               path);

      return 0;
    }

    int                 Crux::Access(const char*                path,
                                     int                        mask)
    {
      elle::String      way(Crux::Mirror + path);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mask);

      if (::access(way.c_str(), mask) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mask);

      return 0;
    }

    int                 Crux::Chmod(const char*                 path,
                                    mode_t                      mode)
    {
      elle::String      way(Crux::Mirror + path);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mode);

      if (::chmod(way.c_str(), mode) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mode);

      return 0;
    }

    int                 Crux::Chown(const char*                 path,
                                    uid_t                       uid,
                                    gid_t                       gid)
    {
      elle::String      way(Crux::Mirror + path);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %u, %u)\n",
               __FUNCTION__,
               path, uid, gid);

      if (::lchown(way.c_str(), uid, gid) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %u, %u)\n",
               __FUNCTION__,
               path, uid, gid);

      return 0;
    }

#if defined(HAVE_SETXATTR)
# if defined(INFINIT_LINUX) && 0
    int                 Crux::Setxattr(const char*              path,
                                       const char*              name,
                                       const char*              value,
                                       size_t                   size,
                                       int                      flags)
    {
      elle::String      way(Crux::Mirror + path);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %s, %p, %u, 0x%x)\n",
               __FUNCTION__,
               path, name, value, size, flags);

      if (::lsetxattr(way.c_str(), name, value, size, flags) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %s, %p, %u, 0x%x)\n",
               __FUNCTION__,
               path, name, value, size, flags);

      return 0;
    }

    int                 Crux::Getxattr(const char*              path,
                                       const char*              name,
                                       char*                    value,
                                       size_t                   size)
    {
      elle::String      way(Crux::Mirror + path);
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %s, %p, %u)\n",
               __FUNCTION__,
               path, name, value, size);

      if ((res = ::lgetxattr(way.c_str(), name, value, size)) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %s, %p, %u)\n",
               __FUNCTION__,
               path, name, value, size);

      return res;
    }

    int                 Crux::Listxattr(const char*             path,
                                        char*                   list,
                                        size_t                  size)
    {
      elle::String      way(Crux::Mirror + path);
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %p, %u)\n",
               __FUNCTION__,
               path, list, size);

      if ((res = ::llistxattr(way.c_str(), list, size)) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %p, %u)\n",
               __FUNCTION__,
               path, list, size);

      return res;
    }

    int                 Crux::Removexattr(const char*           path,
                                          const char*           name)
    {
      elle::String      way(Crux::Mirror + path);
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %s)\n",
               __FUNCTION__,
               path, name);

      if ((res = ::lremovexattr(way.c_str(), name)) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %s)\n",
               __FUNCTION__,
               path, name);

      return res;
    }
# elif defined(INFINIT_MACOSX)
    int                 Crux::Setxattr(const char*              path,
                                       const char*              name,
                                       const char*              value,
                                       size_t                   size,
                                       int                      options,
                                       uint32_t                 position)
    {
      elle::String      way(Crux::Mirror + path);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %s, %p, %lu, 0x%x, %u)\n",
               __FUNCTION__,
               path, name, value, size, options, position);

      if (::setxattr(way.c_str(), name, value, size, position,
                     options | XATTR_NOFOLLOW) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %s, %p, %lu, 0x%x, %u)\n",
               __FUNCTION__,
               path, name, value, size, options, position);

      return 0;
    }

    int                 Crux::Getxattr(const char*              path,
                                       const char*              name,
                                       char*                    value,
                                       size_t                   size,
                                       uint32_t                 position)
    {
      elle::String      way(Crux::Mirror + path);
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %s, %p, %lu, %u)\n",
               __FUNCTION__,
               path, name, value, size, position);

      if ((res = ::getxattr(way.c_str(), name, value, size, position,
                            XATTR_NOFOLLOW)) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %s, %p, %lu, %u)\n",
               __FUNCTION__,
               path, name, value, size, position);

      return res;
    }

    int                 Crux::Listxattr(const char*             path,
                                        char*                   list,
                                        size_t                  size)
    {
      elle::String      way(Crux::Mirror + path);
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %p, %lu)\n",
               __FUNCTION__,
               path, list, size);

      if ((res = ::listxattr(way.c_str(), list, size,
                             XATTR_NOFOLLOW)) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %p, %lu)\n",
               __FUNCTION__,
               path, list, size);

      return res;
    }

    int                 Crux::Removexattr(const char*           path,
                                          const char*           name)
    {
      elle::String      way(Crux::Mirror + path);
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %s)\n",
               __FUNCTION__,
               path, name);

      if ((res = ::removexattr(way.c_str(), name,
                               XATTR_NOFOLLOW)) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %s)\n",
               __FUNCTION__,
               path, name);

      return res;
    }
# endif
#endif

    int                 Crux::Symlink(const char*               to,
                                      const char*               from)
    {
      elle::String      way(Crux::Mirror + from);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %s)\n",
               __FUNCTION__,
               to, from);

      if (::symlink(to, way.c_str()) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %s)\n",
               __FUNCTION__,
               to, from);

      return 0;
    }

    int                 Crux::Readlink(const char*              path,
                                       char*                    buf,
                                       size_t                   size)
    {
      elle::String      way(Crux::Mirror + path);
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %p, %qu)\n",
               __FUNCTION__,
               path, buf, static_cast<elle::Natural64>(size));

      if ((res = ::readlink(way.c_str(), buf, size - 1)) == -1)
        return -errno;

      buf[res] = '\0';

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %p, %qu)\n",
               __FUNCTION__,
               path, buf, static_cast<elle::Natural64>(size));

      return 0;
    }

    int                 Crux::Create(const char*                path,
                                     mode_t                     mode,
                                     struct ::fuse_file_info*   fi)
    {
      elle::String      way(Crux::Mirror + path);
      int               fd;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, 0%o, %p)\n",
               __FUNCTION__,
               path, mode, fi);

      if ((fd = ::open(way.c_str(), fi->flags, mode)) == -1)
        return -errno;

      fi->fh = fd;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, 0%o, %p)\n",
               __FUNCTION__,
               path, mode, fi);

      return 0;
    }

    int                 Crux::Open(const char*                  path,
                                   struct ::fuse_file_info*     fi)
    {
      elle::String      way(Crux::Mirror + path);
      int               fd;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      if ((fd = ::open(way.c_str(), fi->flags)) == -1)
        return -errno;

      fi->fh = fd;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      return 0;
    }

    int                 Crux::Write(const char*                 path,
                                    const char*                 buf,
                                    size_t                      size,
                                    off_t                       offset,
                                    struct ::fuse_file_info*    fi)
    {
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %p, %qu, %qu, %p)\n",
               __FUNCTION__,
               path, buf, static_cast<elle::Natural64>(size),
               static_cast<elle::Natural64>(offset), fi);

      (void)path;

      if ((res = ::pwrite(fi->fh, buf, size, offset)) == -1)
        res = -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %p, %qu, %qu, %p)\n",
               __FUNCTION__,
               path, buf, static_cast<elle::Natural64>(size),
               static_cast<elle::Natural64>(offset), fi);

      return res;
    }

    int                 Crux::Read(const char*                  path,
                                   char*                        buf,
                                   size_t                       size,
                                   off_t                        offset,
                                   struct ::fuse_file_info*     fi)
    {
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %p, %qu, %qu, %p)\n",
               __FUNCTION__,
               path, buf, static_cast<elle::Natural64>(size),
               static_cast<elle::Natural64>(offset), fi);

      (void)path;

      if ((res = ::pread(fi->fh, buf, size, offset)) == -1)
        res = -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %p, %qu, %qu, %p)\n",
               __FUNCTION__,
               path, buf, static_cast<elle::Natural64>(size),
               static_cast<elle::Natural64>(offset), fi);

      return res;
    }

    int                 Crux::Truncate(const char*              path,
                                       off_t                    size)
    {
      elle::String      way(Crux::Mirror + path);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %qu)\n",
               __FUNCTION__,
               path, static_cast<elle::Natural64>(size));

      if (::truncate(way.c_str(), size) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %qu)\n",
               __FUNCTION__,
               path, static_cast<elle::Natural64>(size));

      return 0;
    }

    int                 Crux::Ftruncate(const char*             path,
                                        off_t                   size,
                                        struct ::fuse_file_info* fi)
    {
      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %qu, %p)\n",
               __FUNCTION__,
               path, static_cast<elle::Natural64>(size), fi);

      (void)path;

      if (::ftruncate(fi->fh, size) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %qu, %p)\n",
               __FUNCTION__,
               path, static_cast<elle::Natural64>(size), fi);

      return 0;
    }

    int                 Crux::Release(const char*               path,
                                      struct ::fuse_file_info*  fi)
    {
      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      (void)path;

      ::close(fi->fh);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      return 0;
    }

    int                 Crux::Rename(const char*                from,
                                     const char*                to)
    {
      elle::String      f(Crux::Mirror + from);
      elle::String      t(Crux::Mirror + to);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s, %s)\n",
               __FUNCTION__,
               from, to);

      if (::rename(f.c_str(), t.c_str()) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s, %s)\n",
               __FUNCTION__,
               from, to);

      return 0;
    }

    int                 Crux::Unlink(const char*                path)
    {
      elle::String      way(Crux::Mirror + path);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Crux::%s(%s)\n",
               __FUNCTION__,
               path);

      if (::unlink(way.c_str()) == -1)
        return -errno;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Crux::%s(%s)\n",
               __FUNCTION__,
               path);

      return 0;
    }

    ///
    /// this method sets up the crux.
    ///
    elle::Status        Crux::Setup(const elle::String&         mirror)
    {
      // set the mirror path.
      if (mirror.empty() == false)
        Crux::Mirror = mirror + "/";

      //
      // initialize the FUSE operations.
      //
      {
        // set all the pointers to zero.
        ::memset(&Crux::Operations, 0x0, sizeof (::fuse_operations));

        // Crux::Operations.statfs: not supported
        Crux::Operations.getattr = Crux::Getattr;
        Crux::Operations.fgetattr = Crux::Fgetattr;
        Crux::Operations.utimens = Crux::Utimens;

        Crux::Operations.opendir = Crux::Opendir;
        Crux::Operations.readdir = Crux::Readdir;
        Crux::Operations.releasedir = Crux::Releasedir;
        Crux::Operations.mkdir = Crux::Mkdir;
        Crux::Operations.rmdir = Crux::Rmdir;

        Crux::Operations.access = Crux::Access;
        Crux::Operations.chmod = Crux::Chmod;
        Crux::Operations.chown = Crux::Chown;
#if defined(HAVE_SETXATTR) && 0
        Crux::Operations.setxattr = Crux::Setxattr;
        Crux::Operations.getxattr = Crux::Getxattr;
        Crux::Operations.listxattr = Crux::Listxattr;
        Crux::Operations.removexattr = Crux::Removexattr;
#endif

        // Crux::Operations.link: not supported
        Crux::Operations.readlink = Crux::Readlink;
        Crux::Operations.symlink = Crux::Symlink;

        Crux::Operations.create = Crux::Create;
        // Crux::Operations.mknod: not supported
        Crux::Operations.open = Crux::Open;
        Crux::Operations.write = Crux::Write;
        Crux::Operations.read = Crux::Read;
        Crux::Operations.truncate = Crux::Truncate;
        Crux::Operations.ftruncate = Crux::Ftruncate;
        Crux::Operations.release = Crux::Release;

        Crux::Operations.rename = Crux::Rename;
        Crux::Operations.unlink = Crux::Unlink;

        Crux::Operations.flag_nullpath_ok = 1;
      }

      return elle::Status::Ok;
    }

  }
}
