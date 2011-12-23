//
// ---------- header ----------------------------------------------------------
//
// project       8diary
//
// license       infinit
//
// author        julien quintard   [tue jun 28 12:47:04 2011]
//

#ifndef DIARY_UNIX_CRUX_HH
#define DIARY_UNIX_CRUX_HH

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
# ifdef HAVE_SETXATTR
#  include <sys/xattr.h>
# endif
#include <elle/idiom/Open.hh>

namespace application
{
  namespace unix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains the implementation of the FUSE upcalls.
    ///
    class Crux
    {
    public:
      //
      // structures
      //
      struct Handle
      {
	DIR*		dp;
	struct dirent*	entry;
	off_t		offset;
      };

      //
      // methods
      //

      // general purpose
      static int		Getattr(const char*,
					struct stat*);
      static int		Fgetattr(const char*,
					 struct stat*,
					 struct ::fuse_file_info*);
      static int		Utimens(const char*,
					const struct timespec[2]);

      // directory
      static int		Opendir(const char*,
					struct ::fuse_file_info*);
      static int		Readdir(const char*,
					void*,
					::fuse_fill_dir_t,
					off_t,
					struct ::fuse_file_info*);
      static int		Releasedir(const char*,
					   struct ::fuse_file_info*);
      static int		Mkdir(const char*,
				      mode_t);
      static int		Rmdir(const char*);

      // access
      static int		Access(const char*,
				       int);
      static int		Chmod(const char*,
				      mode_t);
      static int		Chown(const char*,
				      uid_t,
				      gid_t);

#ifdef HAVE_SETXATTR
      // attribute
      static int		Setxattr(const char*,
					 const char*,
					 const char*,
					 size_t,
					 int);
      static int		Getxattr(const char*,
					 const char*,
					 char*,
					 size_t);
      static int		Listxattr(const char*,
					  char*,
					  size_t);
      static int		Removexattr(const char*,
					    const char*);
#endif

      // link
      static int		Symlink(const char*,
					const char*);
      static int		Readlink(const char*,
					 char*,
					 size_t);

      // file
      static int		Create(const char*,
				       mode_t,
				       struct ::fuse_file_info*);
      static int		Open(const char*,
				     struct ::fuse_file_info*);
      static int		Write(const char*,
				      const char*,
				      size_t,
				      off_t,
				      struct ::fuse_file_info*);
      static int		Read(const char*,
				     char*,
				     size_t,
				     off_t,
				     struct ::fuse_file_info*);
      static int		Truncate(const char*,
					 off_t);
      static int		Ftruncate(const char*,
					  off_t,
					  struct ::fuse_file_info*);
      static int		Release(const char*,
					struct ::fuse_file_info*);

      // objects
      static int		Rename(const char*,
				       const char*);
      static int		Unlink(const char*);

      //
      // static methods
      //
      static elle::Status	Setup(const elle::String&);

      //
      // static attributes
      //
      static struct ::fuse_operations	Operations;

      static elle::String		Mirror;
    };

  }
}

#endif
