//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/PIG.hh
//
// created       julien quintard   [fri jul 31 22:11:24 2009]
// updated       julien quintard   [sun feb  7 04:12:08 2010]
//

#ifndef PIG_PIG_HH
#define PIG_PIG_HH

//
// ---------- macros ----------------------------------------------------------
//

#define FUSE_USE_VERSION		26

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <etoile/Etoile.hh>

#include <fuse.h>

namespace pig
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// XXX
  ///
  class PIG
  {
  public:
    //
    // static methods
    //
    static int	getattr(const char*,
			struct stat*);
    static int	setxattr(const char*,
			 const char*,
			 const char*,
			 size_t,
			 int);
    static int	chmod(const char*,
		      mode_t);
    static int	chown(const char*,
		      uid_t,
		      gid_t);

    static int	access(const char*,
		       int);
    static int	utimens(const char*,
			const struct timespec[2]);

    static int	mknod(const char*,
		      mode_t,
		      dev_t);
    static int	open(const char*,
		     struct fuse_file_info*);
    static int	read(const char*,
		     char*,
		     size_t,
		     off_t,
		     struct fuse_file_info*);
    static int	write(const char*,
		      const char*,
		      size_t,
		      off_t,
		      struct fuse_file_info*);
    static int	truncate(const char*,
			 off_t);
    static int	release(const char*,
			struct fuse_file_info*);
    static int	unlink(const char*);
    static int	rename(const char*,
		       const char*);

    static int	symlink(const char*,
			const char*);
    static int	readlink(const char*,
			 char*,
			 size_t);

    static int	mkdir(const char*,
		      mode_t);
    static int	readdir(const char*,
			void*,
			fuse_fill_dir_t,
			off_t,
			struct fuse_file_info*);
    static int	rmdir(const char*);


    //
    // static methods
    //
    static Status	Initialize();
    static Status	Clean();
    static Status	Main(int,
			     char**);
  };

}

#endif
