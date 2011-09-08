//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/Crux.hh
//
// created       julien quintard   [wed jun  1 09:19:13 2011]
// updated       julien quintard   [wed sep  7 12:41:26 2011]
//

#ifndef PIG_CRUX_HH
#define PIG_CRUX_HH

//
// ---------- macros ----------------------------------------------------------
//

#ifndef FUSE_USE_VERSION
# define FUSE_USE_VERSION		26
#endif

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>

#include <elle/idiom/Close.hh>
# include <fuse.h>
# ifdef HAVE_SETXATTR
#  include <attr/xattr.h>
# endif
#include <elle/idiom/Open.hh>

namespace pig
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
    // constants
    //
    static const nucleus::Size		Range;

    //
    // callbacks
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
  };

}

#endif
