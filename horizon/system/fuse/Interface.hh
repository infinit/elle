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
// updated       julien quintard   [mon aug  3 20:48:11 2009]
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
      static int	access(const char*,
			       int);
      static int	mknod(const char*,
			      mode_t,
			      dev_t);
      static int	unlink(const char*);
      static int	utimens(const char*,
				const struct timespec[2]);
      static int	readdir(const char*,
				void*,
				fuse_fill_dir_t,
				off_t,
				struct fuse_file_info*);
      static int	mkdir(const char*,
			      mode_t);
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
