//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [tue jul 26 15:33:54 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <horizon/macosx/ParallelFUker.hh>
#include <horizon/macosx/FUSE.hh>

#include <hole/Hole.hh>

// XXX
#include <elle/idiom/Close.hh>
# include <QCoreApplication>
# include <pthread.h>
// XXX
# include <pth.h>
#include <elle/idiom/Open.hh>

namespace horizon
{
  namespace macosx
  {

//
// ---------- XXX -------------------------------------------------------------
//

    ::pthread_mutex_t MUTEX;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    ParallelFUker::ParallelFUker()
    {
    }

    ///
    /// destructor.
    ///
    ParallelFUker::~ParallelFUker()
    {
    }

//
// ---------- fuker -----------------------------------------------------------
//

    /// XXX
    void* MAIN(void*)
    {
      //
      // build the arguments.
      //
      // note that the -h option can be passed in order to display all
      // the available options including the threaded, debug, file system
      // name, file system type etc.
      //
      // for example the -d option could be used instead of -f in order
      // to activate the debug mode.
      //
      // note that compared to the Linux implementation, the MacOS X
      // FUSE does not support the -o large_read and -o big_writes
      // options.
      //
      elle::String      ofsname("-ofsname=" +
                                hole::Hole::Descriptor.name);
      elle::String      ovolname("-ovolname=" +
                                 hole::Hole::Descriptor.name);
      const char*       arguments[] =
        {
          "horizon",

          "-s", // XXX
          "-d", // XXX

          //
          // this option does not register FUSE as a daemon but
          // run it in foreground.
          //
          "-f",

          //
          // this option indicates the name of the file system type.
          //
          "-ofstypename=infinit",

          //
          // this option disables remote file locking.
          //
          "-ono_remote_lock",

          //
          // this option activates the in-kernel caching based on
          // the modification times.
          //
          "-oauto_cache",

          //
          // this options activates the MacOS X ACLs which are set through
          // extended attributes.
          //
          "-oextended_security",

          //
          // this option forbids the access to and creation of .DS_Store files
          // along with any ._* file.
          //
          "-onoappledouble",

          //
          // these options activate the custom volume icon.
          //
          // note that Apple icon (icns) files can be generated
          // with the _makeicns_ binary.
          //
          // XXX[we should put this file as an output of the
          //     compilation/installation process]
          //
          "-omodules=volicon",
          "-oiconpath=../../temporary/nazi.icns",

          //
          // this option specifies the name of the file system instance.
          //
          ofsname.c_str(),

          //
          // this option provides a user-friendly name for the MacOS X volume.
          //
          ovolname.c_str(),

          //
          // and finally, the mountpoint.
          //
          FUSE::Mountpoint.c_str()
        };

      struct ::fuse_operations          operations;
      {
        // set all the pointers to zero.
        ::memset(&operations, 0x0, sizeof (::fuse_operations));

        operations.statfs = ParallelFUker::Statfs;
        operations.getattr = ParallelFUker::Getattr;
        operations.fgetattr = ParallelFUker::Fgetattr;
        operations.utimens = ParallelFUker::Utimens;

        operations.opendir = ParallelFUker::Opendir;
        operations.readdir = ParallelFUker::Readdir;
        operations.releasedir = ParallelFUker::Releasedir;
        operations.mkdir = ParallelFUker::Mkdir;
        operations.rmdir = ParallelFUker::Rmdir;

        operations.access = ParallelFUker::Access;
        operations.chmod = ParallelFUker::Chmod;
        operations.chown = ParallelFUker::Chown;
#if defined(HAVE_SETXATTR)
        operations.setxattr = ParallelFUker::Setxattr;
        operations.getxattr = ParallelFUker::Getxattr;
        operations.listxattr = ParallelFUker::Listxattr;
        operations.removexattr = ParallelFUker::Removexattr;
#endif

        // operations.link: not supported
        operations.readlink = ParallelFUker::Readlink;
        operations.symlink = ParallelFUker::Symlink;

        operations.create = ParallelFUker::Create;
        // operations.mknod: not supported
        operations.open = ParallelFUker::Open;
        operations.write = ParallelFUker::Write;
        operations.read = ParallelFUker::Read;
        operations.truncate = ParallelFUker::Truncate;
        operations.ftruncate = ParallelFUker::Ftruncate;
        operations.release = ParallelFUker::Release;

        operations.rename = ParallelFUker::Rename;
        operations.unlink = ParallelFUker::Unlink;

        // the following flag being activated prevents the path argument
        // to be passed for functions which take a file descriptor.
        operations.flag_nullpath_ok = 1;
      }

      // XXX
      ::fuse_main(
        sizeof (arguments) / sizeof (elle::Character*),
        const_cast<char**>(arguments),
        &operations,
        NULL);

      return (NULL);
    }

    // XXX
    BROKER* ParallelFUker::Broker = NULL;

    ///
    /// this method sets up the fuker.
    ///
    elle::Status        ParallelFUker::Setup()
    {
      ParallelFUker::Broker = new BROKER;

      // XXX
      if (::pthread_mutex_init(&MUTEX, NULL) != 0)
        escape("XXX");

      // XXX
      static ::pthread_t fuse_thread;

      if (::pthread_create(&fuse_thread, NULL, &MAIN, NULL) != 0)
        escape("XXX");

      return elle::StatusOk;
    }

//
// ---------- XXX -------------------------------------------------------------
//

    elle::Status        STATFS(EVENT* e)
    {
      e->result = FUSE::Operations.statfs(e->path, e->statvfs);

      ::pthread_mutex_unlock(&e->mutex);

      return (elle::StatusOk);
    }

    elle::Status        UTIMENS(EVENT* e)
    {
      e->result = FUSE::Operations.utimens(e->path, e->ts);

      ::pthread_mutex_unlock(&e->mutex);

      return (elle::StatusOk);
    }

    elle::Status        OPENDIR(EVENT* e)
    {
      printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX SLEEP\n");
      elle::Fiber::Sleep(1000);

      e->result = FUSE::Operations.opendir(e->path, e->fi);

      printf("UNBLOCKING %p\n", &e->mutex);
      ::pthread_mutex_unlock(&e->mutex);

      return (elle::StatusOk);
    }

    bool BROKER::event(QEvent* event)
    {
      EVENT* e = static_cast<EVENT*>(event); // XXX tenter un proto avec EVENT

      switch(e->operation)
        {
        case EVENT::OperationStatfs:
          {
            elle::Closure<
              elle::Status,
              elle::Parameters<EVENT*>
              >             closure(elle::Callback<>::Infer(&STATFS),
                                    e);

            if (elle::Fiber::Spawn(closure) == elle::StatusError)
              yield(_(true), "unable to spawn a fiber");

            break;
          }
        case EVENT::OperationUtimens:
          {
            elle::Closure<
              elle::Status,
              elle::Parameters<EVENT*>
              >             closure(elle::Callback<>::Infer(&UTIMENS),
                                    e);

            if (elle::Fiber::Spawn(closure) == elle::StatusError)
              yield(_(true), "unable to spawn a fiber");

            break;
          }
        case EVENT::OperationOpendir:
          {
            elle::Closure<
              elle::Status,
              elle::Parameters<EVENT*>
              >             closure(elle::Callback<>::Infer(&OPENDIR),
                                    e);

            if (elle::Fiber::Spawn(closure) == elle::StatusError)
              yield(_(true), "unable to spawn a fiber");

            break;
          }
        }

      // XXX enlever le big lock puisque chaque thread FUSE va faire un post
      // event ce qui va avoir comme impact de sequentialiser les events.

      return true;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method returns general information on the file system.
    ///
    int                 ParallelFUker::Statfs(const char*       path,
                                              struct ::statvfs* statvfs)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        pthread_mutex_t mutex;
        EVENT* e = new EVENT(mutex, res);

        e->operation = EVENT::OperationStatfs;
        e->path = (char*)path;
        e->statvfs = statvfs;

        ::pthread_mutex_init(&mutex, NULL);

        ::pthread_mutex_lock(&mutex);

        ::QCoreApplication::postEvent(ParallelFUker::Broker, e);

        // XXX peut etre ameliorer car si on lance le postEvent et que direct
        // le boucle traite le truc et que c'est rapide alors on peut avoir
        // le unlock avant le lock bloquant!

        ::pthread_mutex_lock(&mutex);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method returns general-purpose information on the file system
    /// object identified by _path_.
    ///
    int                 ParallelFUker::Getattr(const char*      path,
                                               struct ::stat*   stat)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.getattr(path, stat);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method returns general-purpose information on the file system
    /// object identified by _path_.
    ///
    int                 ParallelFUker::Fgetattr(const char*              path,
                                                struct ::stat*           stat,
                                                struct ::fuse_file_info* info)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.fgetattr(path, stat, info);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method changes the access and modification time of the object.
    ///
    int                 ParallelFUker::Utimens(const char*      path,
                                               const struct ::timespec ts[2])
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        pthread_mutex_t mutex;
        EVENT* e = new EVENT(mutex, res);

        e->operation = EVENT::OperationUtimens;
        e->path = (char*)path;
        e->ts[0] = ts[0];
        e->ts[1] = ts[1];

        ::pthread_mutex_init(&mutex, NULL);

        ::pthread_mutex_lock(&mutex);

        ::QCoreApplication::postEvent(ParallelFUker::Broker, e);

        // XXX peut etre ameliorer car si on lance le postEvent et que direct
        // le boucle traite le truc et que c'est rapide alors on peut avoir
        // le unlock avant le lock bloquant!

        ::pthread_mutex_lock(&mutex);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method opens the directory _path_.
    ///
    int                 ParallelFUker::Opendir(const char*              path,
                                               struct ::fuse_file_info* info)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        pthread_mutex_t mutex;
        EVENT* e = new EVENT(mutex, res);

        e->operation = EVENT::OperationOpendir;
        e->path = (char*)path;
        e->fi = info;

        ::pthread_mutex_init(&mutex, NULL);

        ::pthread_mutex_lock(&mutex);

        ::QCoreApplication::postEvent(ParallelFUker::Broker, e);

        // XXX peut etre ameliorer car si on lance le postEvent et que direct
        // le boucle traite le truc et que c'est rapide alors on peut avoir
        // le unlock avant le lock bloquant!

        printf("BLOCKING %p\n", &mutex);
        ::pthread_mutex_lock(&mutex);
        printf("UNBLOCKED %p\n", &mutex);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method reads the directory entries.
    ///
    int                 ParallelFUker::Readdir(const char*              path,
                                               void*                    buffer,
                                               ::fuse_fill_dir_t        filler,
                                               off_t                    offset,
                                               struct ::fuse_file_info* info)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.readdir(path, buffer, filler, offset, info);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method closes the directory _path_.
    ///
    int                 ParallelFUker::Releasedir(const char*              path,
                                                  struct ::fuse_file_info* info)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.releasedir(path, info);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method creates a directory.
    ///
    int                 ParallelFUker::Mkdir(const char*        path,
                                             mode_t             mode)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.mkdir(path, mode);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method removes a directory.
    ///
    int                 ParallelFUker::Rmdir(const char*        path)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.rmdir(path);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method checks if the current user has the permission to access
    /// the object _path_ for the operations _mask_.
    ///
    int                 ParallelFUker::Access(const char*       path,
                                              int               mask)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.access(path, mask);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method modifies the permissions on the object.
    ///
    int                 ParallelFUker::Chmod(const char*        path,
                                             mode_t             mode)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.chmod(path, mode);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method modifies the owner of a given object.
    ///
    int                 ParallelFUker::Chown(const char*        path,
                                             uid_t              uid,
                                             gid_t              gid)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.chown(path, uid, gid);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

#if defined(HAVE_SETXATTR)
    ///
    /// this method sets an extended attribute value.
    ///
    /// note that the flags are ignored!
    ///
    int                 ParallelFUker::Setxattr(const char*     path,
                                                const char*     name,
                                                const char*     value,
                                                size_t          size,
                                                int             flags,
                                                uint32_t        position)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.setxattr(path, name, value, size, flags, position);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method returns the attribute associated with the given object.
    ///
    int                 ParallelFUker::Getxattr(const char*     path,
                                                const char*     name,
                                                char*           value,
                                                size_t          size,
                                                uint32_t        position)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.getxattr(path, name, value, size, position);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method returns the list of attribute names.
    ///
    int                 ParallelFUker::Listxattr(const char*    path,
                                                 char*          list,
                                                 size_t         size)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.listxattr(path, list, size);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method removes an attribute.
    ///
    int                 ParallelFUker::Removexattr(const char*  path,
                                                   const char*  name)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.removexattr(path, name);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }
#endif

    ///
    /// this method creates a symbolic link.
    ///
    int                 ParallelFUker::Symlink(const char*      target,
                                               const char*      source)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.symlink(target, source);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method returns the target path pointed by the symbolic link.
    ///
    int                 ParallelFUker::Readlink(const char*     path,
                                                char*           buffer,
                                                size_t          size)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.readlink(path, buffer, size);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method creates a new file and opens it.
    ///
    int                 ParallelFUker::Create(const char*              path,
                                              mode_t                   mode,
                                              struct ::fuse_file_info* info)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.create(path, mode, info);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method opens a file.
    ///
    int                 ParallelFUker::Open(const char*              path,
                                            struct ::fuse_file_info* info)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.open(path, info);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method writes data to a file.
    ///
    int                 ParallelFUker::Write(const char*              path,
                                             const char*              buffer,
                                             size_t                   size,
                                             off_t                    offset,
                                             struct ::fuse_file_info* info)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.write(path, buffer, size, offset, info);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method reads data from a file.
    ///
    int                 ParallelFUker::Read(const char*              path,
                                            char*                    buffer,
                                            size_t                   size,
                                            off_t                    offset,
                                            struct ::fuse_file_info* info)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.read(path, buffer, size, offset, info);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method modifies the size of a file.
    ///
    int                 ParallelFUker::Truncate(const char*     path,
                                                off_t           size)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.truncate(path, size);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method modifies the size of an opened file.
    ///
    int                 ParallelFUker::Ftruncate(const char*              path,
                                                 off_t                    size,
                                                 struct ::fuse_file_info* info)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.ftruncate(path, size, info);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method closes a file.
    ///
    int                 ParallelFUker::Release(const char*              path,
                                               struct ::fuse_file_info* info)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.release(path, info);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method renames a file.
    ///
    int                 ParallelFUker::Rename(const char*       source,
                                              const char*       target)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.rename(source, target);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

    ///
    /// this method removes an existing file.
    ///
    int                 ParallelFUker::Unlink(const char*       path)
    {
      int               res;

      ::pthread_mutex_lock(&MUTEX);
      {
        res = FUSE::Operations.unlink(path);
      }
      ::pthread_mutex_unlock(&MUTEX);

      return (res);
    }

  }
}
