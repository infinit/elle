//
// ---------- header ----------------------------------------------------------
//
// project       diary
//
// license       infinit
//
// author        julien quintard   [thu jun 30 09:19:53 2011]
//

#ifndef DIARY_UNIX_REPLAY_HH
#define DIARY_UNIX_REPLAY_HH

//
// ---------- definitions -----------------------------------------------------
//

#ifndef FUSE_USE_VERSION
# define FUSE_USE_VERSION 26
#endif

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <satellites/diary/unix/Upcall.hh>

#include <elle/idiom/Close.hh>
# if defined(INFINIT_MACOSX)
#  define _GNU_SOURCE
# endif
# include <fuse.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <dirent.h>
# include <errno.h>
# include <sys/time.h>
# include <sys/stat.h>
# if defined(HAVE_SETXATTR)
#  if defined(INFINIT_LINUX)
#   include <attr/xattr.h>
#  elif defined(INFINIT_MACOSX)
#   include <sys/xattr.h>
#  else
#   error "unsupported platform"
#  endif
# endif
#include <elle/idiom/Open.hh>

///
/// the HAVE_SETXATTR macro is undefined because the calls crash on MacOS X.
///
#if defined(INFINIT_MACOSX)
# undef HAVE_SETXATTR
#endif

namespace satellite
{
  #undef unix
  namespace unix
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// this class must be forward-declared.
    ///
    class Memoirs;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the replaying session.
    ///
    class Replay
    {
    public:
      //
      // static methods
      //
      static elle::Status       Getattr(Upcall&);
      static elle::Status       Fgetattr(Upcall&);
      static elle::Status       Utimens(Upcall&);
      static elle::Status       Opendir(Upcall&);
      static elle::Status       Readdir(Upcall&);
      static elle::Status       Releasedir(Upcall&);
      static elle::Status       Mkdir(Upcall&);
      static elle::Status       Rmdir(Upcall&);
      static elle::Status       Access(Upcall&);
      static elle::Status       Chmod(Upcall&);
      static elle::Status       Chown(Upcall&);

#if defined(HAVE_SETXATTR)
      static elle::Status       Setxattr(Upcall&);
      static elle::Status       Getxattr(Upcall&);
      static elle::Status       Listxattr(Upcall&);
      static elle::Status       Removexattr(Upcall&);
#endif

      static elle::Status       Symlink(Upcall&);
      static elle::Status       Readlink(Upcall&);
      static elle::Status       Create(Upcall&);
      static elle::Status       Open(Upcall&);
      static elle::Status       Write(Upcall&);
      static elle::Status       Read(Upcall&);
      static elle::Status       Truncate(Upcall&);
      static elle::Status       Ftruncate(Upcall&);
      static elle::Status       Release(Upcall&);
      static elle::Status       Rename(Upcall&);
      static elle::Status       Unlink(Upcall&);

      static elle::Status       Initialize(Memoirs*);
      static elle::Status       Clean();

      static elle::Status       Process();

      //
      // static attributes
      //
      static Memoirs*           Reference;
      static elle::Entrance<
        elle::Status::,
        elle::Parameters<>
        >*                      Entrance;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <satellites/diary/unix/Memoirs.hh>

#endif
