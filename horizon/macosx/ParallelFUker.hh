//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [tue jul 26 15:33:15 2011]
//

#ifndef HORIZON_MACOSX_PARALLELFUKER_HH
#define HORIZON_MACOSX_PARALLELFUKER_HH

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

#include <horizon/macosx/FUker.hh>

#include <elle/idiom/Close.hh>
# include <fuse/fuse.h>
#include <elle/idiom/Open.hh>

namespace horizon
{
  namespace macosx
  {

//
// ---------- classes ---------------------------------------------------------
//

    class EVENT:
      public QEvent
    {
    public:
      EVENT(pthread_mutex_t& mutex,
            int& result):
        QEvent(::QEvent::User),
        mutex(mutex),
        result(result)
      {
      }

      enum Operation
        {
          OperationStatfs,
          OperationUtimens,
          OperationOpendir
        };

      int operation;

      char* path;
      struct statvfs* statvfs;
      struct stat* stat;
      fuse_file_info* fi;
      struct timespec ts[2];
      void* buffer;
      fuse_fill_dir_t fill;
      off_t offset;
      mode_t mode;
      int mask;
      uid_t uid;
      gid_t gid;
      char* name;
      char* value;
      size_t size;
      int flags;
      uint32_t position;
      char* list;
      char* source;
      char* target;

      pthread_mutex_t& mutex;
      int& result;
    };

    /// XXX
    class BROKER:
      public ::QObject
    {
      // XXX
      Q_OBJECT;

    public:
      elle::Status Event(EVENT*);
      bool event(QEvent*);
    };

    ///
    /// XXX to implement.
    ///
    class ParallelFUker:
      public FUker
    {
    public:
      //
      // constructors & destructors
      //
      ParallelFUker();
      ~ParallelFUker();

      //
      // interfaces
      //

      // fuker
      elle::Status      Setup();

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

      // XXX
      static BROKER* Broker;
    };

  }
}

#endif
