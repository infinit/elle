//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/system/fuse/Interface.hh
//
// created       julien quintard   [fri jul 31 22:39:40 2009]
// updated       julien quintard   [tue aug  4 21:36:25 2009]
//

#ifndef PIG_SYSTEM_FUSE_INTERFACE_HH
#define PIG_SYSTEM_FUSE_INTERFACE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <etoile/Etoile.hh>

#include <pig/util/Path.hh>
#include <pig/util/File.hh>
#include <pig/util/Directory.hh>
#include <pig/util/Link.hh>

#include <fuse.h>

namespace pig
{
  using namespace util;

  namespace system
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Interface
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
      // static attributes
      //
      // XXX[temoprary]
      static KeyPair	User;
    };

  }
}

#endif
