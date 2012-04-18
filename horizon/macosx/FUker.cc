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

#include <horizon/macosx/FUSE.hh>

#include <hole/Hole.hh>

#include <Infinit.hh>

#include <elle/idiom/Close.hh>
# include <boost/interprocess/sync/interprocess_semaphore.hpp>
# include <QCoreApplication>
# include <pthread.h>
# include <sys/param.h>
# include <sys/mount.h>
#include <elle/idiom/Open.hh>

namespace horizon
{
  namespace macosx
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this agent represents the broker which is responsible for receiving
    /// the events once posted to the event loop so as to treat them.
    ///
    Broker*                     FUker::Agent = nullptr;

    ///
    /// this is the identifier of the thread which is spawn so as to start
    /// FUSE.
    ///
    /// note that creating a specific thread is required because the call
    /// to fuse_main() never returns.
    ///
    ::pthread_t                 FUker::Thread;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method represents the entry point of the FUSE-specific thread.
    ///
    /// this method is responsible for starting FUSE.
    ///
    void*               FUker::Setup(void*)
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
          Infinit::Mountpoint.c_str()
        };

      struct ::fuse_operations          operations;
      {
        // set all the pointers to zero.
        ::memset(&operations, 0x0, sizeof (::fuse_operations));

        operations.statfs = FUker::Statfs;
        operations.getattr = FUker::Getattr;
        operations.fgetattr = FUker::Fgetattr;
        operations.utimens = FUker::Utimens;

        operations.opendir = FUker::Opendir;
        operations.readdir = FUker::Readdir;
        operations.releasedir = FUker::Releasedir;
        operations.mkdir = FUker::Mkdir;
        operations.rmdir = FUker::Rmdir;

        operations.access = FUker::Access;
        operations.chmod = FUker::Chmod;
        operations.chown = FUker::Chown;
#if defined(HAVE_SETXATTR)
        operations.setxattr = FUker::Setxattr;
        operations.getxattr = FUker::Getxattr;
        operations.listxattr = FUker::Listxattr;
        operations.removexattr = FUker::Removexattr;
#endif

        // operations.link: not supported
        operations.readlink = FUker::Readlink;
        operations.symlink = FUker::Symlink;

        operations.create = FUker::Create;
        // operations.mknod: not supported
        operations.open = FUker::Open;
        operations.write = FUker::Write;
        operations.read = FUker::Read;
        operations.truncate = FUker::Truncate;
        operations.ftruncate = FUker::Ftruncate;
        operations.release = FUker::Release;

        operations.rename = FUker::Rename;
        operations.unlink = FUker::Unlink;

        // the following flag being activated prevents the path argument
        // to be passed for functions which take a file descriptor.
        operations.flag_nullpath_ok = 1;
      }

      // finally, initialize FUSE.
      if (::fuse_main(
            sizeof (arguments) / sizeof (elle::Character*),
            const_cast<char**>(arguments),
            &operations,
            NULL) != 0)
        log(::strerror(errno));

      // now that FUSE has stopped, make sure the program is exiting.
      if (elle::concurrency::Program::Exit() == elle::Status::Error)
        log("unable to exit the program");

      return NULL;
    }

    ///
    /// XXX[to replace by the new signal mechanism
    ///
    elle::Status        FUker::Run()
    {
      // XXX
      printf("FUker::Run\n");

      // create the FUSE-specific thread.
      if (::pthread_create(&FUker::Thread, NULL, &FUker::Setup, NULL) != 0)
        escape("unable to create the FUSE-specific thread");

      return elle::Status::Ok;
    }

    ///
    /// this method initializes the FUker by allocating a broker
    /// for handling the posted events along with creating a specific
    /// thread for FUSE.
    ///
    elle::Status        FUker::Initialize()
    {
      // allocate the broker.
      FUker::Agent = new Broker;

      // XXX[to replace by the new signal mechanism]
      switch (hole::Hole::state)
        {
        case hole::Hole::StateOffline:
          {
            if (hole::Hole::ready.Subscribe(
                  elle::concurrency::Callback<>::Infer(&FUker::Run)) == elle::Status::Error)
              escape("unable to subscribe to the signal");

            break;
          }
        case hole::Hole::StateOnline:
          {
            if (FUker::Run() == elle::Status::Error)
              escape("unable to run the FUker thread");

            break;
          }
        }

      return elle::Status::Ok;
    }

    ///
    /// this method cleans the FUker by making sure FUSE exits.
    ///
    elle::Status        FUker::Clean()
    {
      // unmount the file system.
      //
      // this operation will normally make FUSE exit.
      ::unmount(Infinit::Mountpoint.c_str(), MNT_FORCE);

      return elle::Status::Ok;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// the callbacks below are triggered by FUSE whenever a kernel event
    /// occurs.
    ///
    /// note that every one of the callbacks run in a specific thread.
    ///
    /// the purpose of the code is to create an event, inject it in
    /// the event loop so that the Broker can treat it in a fiber and
    /// possible block.
    ///
    /// since the thread must not return until the event is treated,
    /// the following relies on a semaphore by blocking on it. once
    /// the event handled, the semaphore is unlocked, in which case
    /// the thread is resumed and can terminate by returning the
    /// result of the upcall.
    ///

    int                         FUker::Statfs(const char*       path,
                                              struct ::statvfs* statvfs)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationStatfs;
      event->path = path;
      event->statvfs = statvfs;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Getattr(const char*      path,
                                               struct ::stat*   stat)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationGetattr;
      event->path = path;
      event->stat = stat;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Fgetattr(const char*              path,
                                                struct ::stat*           stat,
                                                struct ::fuse_file_info* info)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationFgetattr;
      event->path = path;
      event->stat = stat;
      event->info = info;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Utimens(const char*      path,
                                               const struct ::timespec ts[2])
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationUtimens;
      event->path = path;
      event->ts[0] = ts[0];
      event->ts[1] = ts[1];

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Opendir(const char*              path,
                                               struct ::fuse_file_info* info)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationOpendir;
      event->path = path;
      event->info = info;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Readdir(const char*              path,
                                               void*                    buffer,
                                               ::fuse_fill_dir_t        filler,
                                               off_t                    offset,
                                               struct ::fuse_file_info* info)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationReaddir;
      event->path = path;
      event->buffer = buffer;
      event->filler = filler;
      event->offset = offset;
      event->info = info;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Releasedir(const char*              path,
                                                  struct ::fuse_file_info* info)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationReleasedir;
      event->path = path;
      event->info = info;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Mkdir(const char*        path,
                                             mode_t             mode)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationMkdir;
      event->path = path;
      event->mode = mode;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Rmdir(const char*        path)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationRmdir;
      event->path = path;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Access(const char*       path,
                                              int               mask)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationAccess;
      event->path = path;
      event->mask = mask;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Chmod(const char*        path,
                                             mode_t             mode)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationChmod;
      event->path = path;
      event->mode = mode;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Chown(const char*        path,
                                             uid_t              uid,
                                             gid_t              gid)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationChown;
      event->path = path;
      event->uid = uid;
      event->gid = gid;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

#if defined(HAVE_SETXATTR)
    int                         FUker::Setxattr(const char*     path,
                                                const char*     name,
                                                const char*     value,
                                                size_t          size,
                                                int             flags,
                                                uint32_t        position)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationSetxattr;
      event->path = path;
      event->name = name;
      event->value = const_cast<char*>(value);
      event->size = size;
      event->flags = flags;
      event->position = position;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Getxattr(const char*     path,
                                                const char*     name,
                                                char*           value,
                                                size_t          size,
                                                uint32_t        position)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationGetxattr;
      event->path = path;
      event->name = name;
      event->value = value;
      event->size = size;
      event->position = position;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Listxattr(const char*    path,
                                                 char*          list,
                                                 size_t         size)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationListxattr;
      event->path = path;
      event->list = list;
      event->size = size;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Removexattr(const char*  path,
                                                   const char*  name)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationRemovexattr;
      event->path = path;
      event->name = name;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }
#endif

    int                         FUker::Symlink(const char*      target,
                                               const char*      source)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationSymlink;
      event->target = target;
      event->source = source;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Readlink(const char*     path,
                                                char*           buffer,
                                                size_t          size)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationReadlink;
      event->path = path;
      event->buffer = buffer;
      event->size = size;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Create(const char*              path,
                                              mode_t                   mode,
                                              struct ::fuse_file_info* info)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationCreate;
      event->path = path;
      event->mode = mode;
      event->info = info;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Open(const char*              path,
                                            struct ::fuse_file_info* info)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationOpen;
      event->path = path;
      event->info = info;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Write(const char*              path,
                                             const char*              buffer,
                                             size_t                   size,
                                             off_t                    offset,
                                             struct ::fuse_file_info* info)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationWrite;
      event->path = path;
      event->buffer = const_cast<void*>(static_cast<const void*>(buffer));
      event->size = size;
      event->offset = offset;
      event->info = info;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Read(const char*              path,
                                            char*                    buffer,
                                            size_t                   size,
                                            off_t                    offset,
                                            struct ::fuse_file_info* info)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationRead;
      event->path = path;
      event->buffer = buffer;
      event->size = size;
      event->offset = offset;
      event->info = info;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Truncate(const char*     path,
                                                off_t           size)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationTruncate;
      event->path = path;
      event->size = size;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Ftruncate(const char*              path,
                                                 off_t                    size,
                                                 struct ::fuse_file_info* info)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationFtruncate;
      event->path = path;
      event->size = size;
      event->info = info;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Release(const char*              path,
                                               struct ::fuse_file_info* info)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationRelease;
      event->path = path;
      event->info = info;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Rename(const char*       source,
                                              const char*       target)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationRename;
      event->source = source;
      event->target = target;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

    int                         FUker::Unlink(const char*       path)
    {
      boost::interprocess::interprocess_semaphore     semaphore(0);
      Event*                                          event;
      int                                             result;

      // allocate an event.
      event = new Event(&semaphore, &result);

      event->operation = Event::OperationUnlink;
      event->path = path;

      // post the event.
      ::QCoreApplication::postEvent(FUker::Agent, event);

      // finally, wait for the event to be processed.
      semaphore.wait();

      return (result);
    }

  }
}
