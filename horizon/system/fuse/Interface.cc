//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/system/fuse/Interface.cc
//
// created       julien quintard   [fri jul 31 22:47:18 2009]
// updated       julien quintard   [sat aug  1 21:27:19 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/system/fuse/Interface.hh>

namespace pig
{
  namespace system
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// these methods represent the FUSE interface.
    ///

    int			Interface::getattr(const char*		path,
					   struct stat*		stat)
    {
      Address		address;
      File		file;
      String		route(path);

      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      // clear the stat structure.
      ::memset(stat, 0x0, sizeof(struct stat));

      // resolve the path.
      if (util::Path::Resolve(route, address) == StatusError)
	return (-errno);

      // retrieve the file object.
      //if (util::File::Get(XXX, file) == StatusError)
      //return (-errno);

      // fill the stat structure.

      return (0);
    }

    int			Interface::access(const char*		path,
					  int			mask)
    {
      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      return (0);
    }

    int			Interface::mknod(const char*		path,
					 mode_t			mode,
					 dev_t			dev)
    {
      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      return (0);
    }

    int			Interface::unlink(const char*		path)
    {
      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      return (0);
    }

    int			Interface::utimens(const char*		path,
					   const struct timespec tv[2])
    {
      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      return (0);
    }

    int			Interface::readdir(const char*		path,
					   void*		buffer,
					   fuse_fill_dir_t	filler,
					   off_t		offset,
					   struct fuse_file_info* info)
    {
      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      return (0);
    }

    int			Interface::mkdir(const char*		path,
					 mode_t			mode)
    {
      printf("[XXX] %s\n", __PRETTY_FUNCTION__);

      return (0);
    }

  }
}
