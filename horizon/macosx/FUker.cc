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
# include <boost/function.hpp>
# include <boost/interprocess/sync/interprocess_semaphore.hpp>
# include <boost/preprocessor/seq/for_each.hpp>
# include <boost/preprocessor/seq/for_each_i.hpp>
# include <boost/preprocessor/seq/pop_front.hpp>
# include <QCoreApplication>
# include <pthread.h>
# include <sys/param.h>
# include <sys/mount.h>
# include <reactor/scheduler.hh>
# include <reactor/thread.hh>
#include <elle/idiom/Open.hh>

namespace horizon
{
  namespace macosx
  {
    typedef struct ::timespec timespec2[2];

#if defined(HAVE_SETXATTR)
# define INFINIT_FUSE_COMMANDS_XATTR                                    \
    ((setxattr, (const char*)(const char*)(const char*)                 \
      (size_t)(int)(uint32_t)))                                         \
    ((getxattr, (const char*)(const char*)(char*)(size_t)(uint32_t)))   \
    ((listxattr, (const char*)(char*)(size_t)))                         \
    ((removexattr, (const char*)(const char*)))
#else
# define INFINIT_FUSE_COMMANDS_XATTR
#endif

#define INFINIT_FUSE_COMMANDS                                           \
    ((statfs, (const char*)(struct ::statvfs*)))                        \
    ((getattr, (const char*)(struct ::stat*)))                          \
    ((fgetattr, (const char*)(struct ::stat*)                           \
      (struct ::fuse_file_info*)))                                      \
    ((utimens, (const char*)(const timespec2)))                         \
    ((opendir, (const char*)(struct ::fuse_file_info*)))                \
    ((readdir, (const char*)(void*) (::fuse_fill_dir_t)(off_t)          \
      (struct ::fuse_file_info*)))                                      \
    ((releasedir, (const char*)(struct ::fuse_file_info*)))             \
    ((mkdir, (const char*)(mode_t)))                                    \
    ((rmdir, (const char*)))                                            \
    ((access, (const char*)(int)))                                      \
    ((chmod, (const char*)(mode_t)))                                    \
    ((chown, (const char*)(uid_t)(gid_t)))                              \
    INFINIT_FUSE_COMMANDS_XATTR                                         \
    ((symlink, (const char*)(const char*)))                             \
    ((readlink, (const char*)(char*)(size_t)))                          \
    ((create, (const char*)(mode_t)(struct ::fuse_file_info*)))         \
    ((open, (const char*)(struct ::fuse_file_info*)))                   \
    ((write, (const char*)(const char*)(size_t)(off_t)                  \
      (struct ::fuse_file_info*)))                                      \
    ((read, (const char*)(char*)(size_t)(off_t)                         \
      (struct ::fuse_file_info*)))                                      \
    ((truncate, (const char*)(off_t)))                                  \
    ((ftruncate, (const char*)(off_t)(struct ::fuse_file_info*)))       \
    ((release, (const char*)(struct ::fuse_file_info*)))                \
    ((rename, (const char*)(const char*)))                              \
    ((unlink, (const char*)))                                           \

    ///
    /// this is the identifier of the thread which is spawn so as to start
    /// FUSE.
    ///
    /// note that creating a specific thread is required because the call
    /// to fuse_main() never returns.
    ///
    ::pthread_t                 FUker::Thread;

    /// The callbacks below are triggered by FUSE whenever a kernel
    /// event occurs.
    ///
    /// Note that every one of the callbacks run in a specific thread.
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

#define INFINIT_FUSE_FORMALS(R, Data, I, Elem)  \
    , Elem BOOST_PP_CAT(a, BOOST_PP_INC(I))

#define INFINIT_FUSE_EFFECTIVE(R, Data, I, Elem)        \
    , BOOST_PP_CAT(a, I)

#define INFINIT_FUSE_BOUNCER(Name, Args)                                \
    static int                                                          \
    Name(BOOST_PP_SEQ_HEAD(Args) a0                                     \
         BOOST_PP_SEQ_FOR_EACH_I(INFINIT_FUSE_FORMALS, _,               \
                                 BOOST_PP_SEQ_POP_FRONT(Args)))         \
    {                                                                   \
      return elle::concurrency::scheduler().mt_run<int>                 \
        (#Name,                                                         \
         boost::bind(FUSE::Operations.Name                              \
                     BOOST_PP_SEQ_FOR_EACH_I(INFINIT_FUSE_EFFECTIVE,    \
                                             _, Args)));                \
    }                                                                   \

#define INFINIT_FUSE_BOUNCER_(R, Data, Elem)                            \
    INFINIT_FUSE_BOUNCER(BOOST_PP_TUPLE_ELEM(2, 0, Elem),               \
                         BOOST_PP_TUPLE_ELEM(2, 1, Elem))               \

    BOOST_PP_SEQ_FOR_EACH(INFINIT_FUSE_BOUNCER_, _, INFINIT_FUSE_COMMANDS)

#undef INFINIT_FUSE_BOUNCER_
#undef INFINIT_FUSE_BOUNCER
#undef INFINIT_FUSE_FORMALS
#undef INFINIT_FUSE_EFFECTIVE

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
          // XXX
          //
          // XXX "-odaemon_timeout=120",

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
#if 0 // XXX[to change]
          "-omodules=volicon",
          "-oiconpath=../../temporary/nazi.icns",
#endif

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

        // Fill fuse functions array.
#define INFINIT_FUSE_LINK(Name) operations.Name = Name
#define INFINIT_FUSE_LINK_(R, Data, Elem)                       \
        INFINIT_FUSE_LINK(BOOST_PP_TUPLE_ELEM(2, 0, Elem));
        BOOST_PP_SEQ_FOR_EACH(INFINIT_FUSE_LINK_, _, INFINIT_FUSE_COMMANDS)
#undef INFINIT_FUSE_LINK_
#undef INFINIT_FUSE_LINK

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
      elle::Program::Exit();

      return NULL;
    }

    ///
    /// XXX[to replace by the new signal mechanism
    ///
    elle::Status        FUker::Run()
    {
      // create the FUSE-specific thread.
      if (::pthread_create(&FUker::Thread, NULL, &FUker::Setup, NULL) != 0)
        escape("unable to create the FUSE-specific thread");

      return elle::StatusOk;
    }

    ///
    /// this method initializes the FUker by allocating a broker
    /// for handling the posted events along with creating a specific
    /// thread for FUSE.
    ///
    elle::Status        FUker::Initialize()
    {
      // XXX[to do on Linux too i.e signal to start FUSE]

      // XXX[to replace by the new signal mechanism]
      switch (hole::Hole::state)
        {
        case hole::Hole::StateOffline:
          {
            if (hole::Hole::ready.Subscribe(
                  elle::Callback<>::Infer(&FUker::Run)) == elle::StatusError)
              escape("unable to subscribe to the signal");

            break;
          }
        case hole::Hole::StateOnline:
          {
            if (FUker::Run() == elle::StatusError)
              escape("unable to run the FUker thread");

            break;
          }
        }

      return elle::StatusOk;
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

      return elle::StatusOk;
    }
  }
}
