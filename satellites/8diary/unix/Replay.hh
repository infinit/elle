//
// ---------- header ----------------------------------------------------------
//
// project       8diary
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

#include <elle/Elle.hh>

#include <applications/8diary/unix/Upcall.hh>

#include <elle/idiom/Close.hh>
# include <fuse.h>
# include <ulockmgr.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <dirent.h>
# include <errno.h>
# include <sys/time.h>
# include <sys/stat.h>
# ifdef HAVE_SETXATTR
#  include <sys/xattr.h>
# endif
#include <elle/idiom/Open.hh>

namespace application
{
#undef unix
  namespace unix
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Memoirs;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Replay
    {
    public:
      //
      // static methods
      //
      static elle::Status	Getattr(Upcall&);
      static elle::Status	Fgetattr(Upcall&);
      static elle::Status	Utimens(Upcall&);
      static elle::Status	Opendir(Upcall&);
      static elle::Status	Readdir(Upcall&);
      static elle::Status	Releasedir(Upcall&);
      static elle::Status	Mkdir(Upcall&);
      static elle::Status	Rmdir(Upcall&);
      static elle::Status	Access(Upcall&);
      static elle::Status	Chmod(Upcall&);
      static elle::Status	Chown(Upcall&);
      static elle::Status	Setxattr(Upcall&);
      static elle::Status	Getxattr(Upcall&);
      static elle::Status	Listxattr(Upcall&);
      static elle::Status	Removexattr(Upcall&);
      static elle::Status	Symlink(Upcall&);
      static elle::Status	Readlink(Upcall&);
      static elle::Status	Create(Upcall&);
      static elle::Status	Open(Upcall&);
      static elle::Status	Write(Upcall&);
      static elle::Status	Read(Upcall&);
      static elle::Status	Truncate(Upcall&);
      static elle::Status	Ftruncate(Upcall&);
      static elle::Status	Release(Upcall&);
      static elle::Status	Rename(Upcall&);
      static elle::Status	Unlink(Upcall&);

      static elle::Status	Initialize(Memoirs*);
      static elle::Status	Launch(const elle::Natural32,
				       const elle::Natural32);
      static elle::Status	Process();
      static elle::Status	Clean();

      //
      // static attributes
      //
      static Memoirs*		Reference;

      static elle::Natural32	From;
      static elle::Natural32	To;

      static elle::Timer*	Timer;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8diary/unix/Memoirs.hh>

#endif
