//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [tue jul 26 15:28:37 2011]
//

#ifndef HORIZON_MACOSX_FUKER_HH
#define HORIZON_MACOSX_FUKER_HH

//
// ---------- macros ----------------------------------------------------------
//

#ifndef FUSE_USE_VERSION
# define FUSE_USE_VERSION               26
#endif

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <horizon/macosx/Broker.hh>

#include <elle/idiom/Close.hh>
# define _GNU_SOURCE
# include <fuse.h>
# if defined(HAVE_SETXATTR)
#  include <sys/xattr.h>
# endif
#include <elle/idiom/Open.hh>

namespace horizon
{
  namespace macosx
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class abstracts the notion of FUSE-Broker i.e FUker.
    ///
    /// the broker is responsible for converting the spawn thread-based
    /// FUSE upcalls into events for the main event loop.
    ///
    /// the way to do this is to inject an event into the event loop by
    /// passing the upcall argument through the Event instance. then,
    /// the upcall-specific thread blocks on a semaphore because it must
    /// not return befor the event has been treated. once the event treated,
    /// i.e through the Broker, the semaphore is unlocked, enabling the
    /// thread to resume and terminate by returning to the kernel the
    /// result of the operation.
    ///
    class FUker:
      public elle::Entity
    {
    public:
      //
      // static methods
      //
      static void*              Setup(void*);
      static elle::Status       Run();

      static elle::Status       Initialize();
      static elle::Status       Clean();

      //
      // static attributes
      //
      static Broker*            Agent;
      static ::pthread_t        Thread;

      //
      // callbacks
      //

      // general purpose
      static int        Statfs(const char*,
                               struct ::statvfs*);
      static int        Getattr(const char*,
                                struct ::stat*);
      static int        Fgetattr(const char*,
                                 struct ::stat*,
                                 struct ::fuse_file_info*);
      static int        Utimens(const char*,
                                const struct ::timespec[2]);

      // directory
      static int        Opendir(const char*,
                                struct ::fuse_file_info*);
      static int        Readdir(const char*,
                                void*,
                                ::fuse_fill_dir_t,
                                off_t,
                                struct ::fuse_file_info*);
      static int        Releasedir(const char*,
                                   struct ::fuse_file_info*);
      static int        Mkdir(const char*,
                              mode_t);
      static int        Rmdir(const char*);

      // access
      static int        Access(const char*,
                               int);
      static int        Chmod(const char*,
                              mode_t);
      static int        Chown(const char*,
                              uid_t,
                              gid_t);

#if defined(HAVE_SETXATTR)
      // attribute
      static int        Setxattr(const char*,
                                 const char*,
                                 const char*,
                                 size_t,
                                 int,
                                 uint32_t);
      static int        Getxattr(const char*,
                                 const char*,
                                 char*,
                                 size_t,
                                 uint32_t);
      static int        Listxattr(const char*,
                                  char*,
                                  size_t);
      static int        Removexattr(const char*,
                                    const char*);
#endif

      // link
      static int        Symlink(const char*,
                                const char*);
      static int        Readlink(const char*,
                                 char*,
                                 size_t);

      // file
      static int        Create(const char*,
                               mode_t,
                               struct ::fuse_file_info*);
      static int        Open(const char*,
                             struct ::fuse_file_info*);
      static int        Write(const char*,
                              const char*,
                              size_t,
                              off_t,
                              struct ::fuse_file_info*);
      static int        Read(const char*,
                             char*,
                             size_t,
                             off_t,
                             struct ::fuse_file_info*);
      static int        Truncate(const char*,
                                 off_t);
      static int        Ftruncate(const char*,
                                  off_t,
                                  struct ::fuse_file_info*);
      static int        Release(const char*,
                                struct ::fuse_file_info*);

      // objects
      static int        Rename(const char*,
                               const char*);
      static int        Unlink(const char*);
    };

  }
}

#endif
