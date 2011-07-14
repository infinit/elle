//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/diary/Replay.hh
//
// created       julien quintard   [thu jun 30 09:19:53 2011]
// updated       julien quintard   [tue jul 12 07:15:26 2011]
//

#ifndef PIG_DIARY_REPLAY_HH
#define PIG_DIARY_REPLAY_HH

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

#include <pig/diary/Upcall.hh>

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

namespace pig
{
  namespace diary
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Diary;

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

      static elle::Status	Initialize(Diary*);
      static elle::Status	Launch(const elle::Natural32,
				       const elle::Natural32);
      static elle::Status	Process();
      static elle::Status	Clean();

      //
      // static attributes
      //
      static Diary*		Reference;

      static elle::Natural32	From;
      static elle::Natural32	To;

      static elle::Timer	Timer;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <pig/diary/Diary.hh>

#endif
