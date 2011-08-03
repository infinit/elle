//
// ---------- header ----------------------------------------------------------
//
// project       8diary
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8diary/Crux.cc
//
// created       julien quintard   [tue jun 28 12:46:13 2011]
// updated       julien quintard   [sun jul 31 13:32:47 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8diary/Crux.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this structure contains the function pointers to the FUSE
  /// implementation routines.
  ///
  struct ::fuse_operations		Crux::Operations;

  ///
  /// this variable contains the path to the target directory which
  /// is made accessible i.e mirrored through the FUSE file system.
  ///
  elle::String				Crux::Mirror;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// XXX
  ///
  int			Crux::Getattr(const char*		path,
				      struct ::stat*		stbuf)
  {
    elle::String	way(Crux::Mirror + path);

    if (::lstat(way.c_str(), stbuf) == -1)
      return -errno;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Fgetattr(const char*		path,
				       struct ::stat*		stbuf,
				       struct ::fuse_file_info*	fi)
  {
    (void)path;

    if (::fstat(fi->fh, stbuf) == -1)
      return -errno;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Utimens(const char*		path,
				      const struct ::timespec	ts[2])
  {
    elle::String	way(Crux::Mirror + path);
    struct ::timeval	tv[2];

    tv[0].tv_sec = ts[0].tv_sec;
    tv[0].tv_usec = ts[0].tv_nsec / 1000;
    tv[1].tv_sec = ts[1].tv_sec;
    tv[1].tv_usec = ts[1].tv_nsec / 1000;

    if (::utimes(way.c_str(), tv) == -1)
      return -errno;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Opendir(const char*		path,
				      struct ::fuse_file_info*	fi)
  {
    elle::String	way(Crux::Mirror + path);
    int			res;
    Crux::Handle*	d;

    if ((d =
	 (Crux::Handle*)::malloc(sizeof(Crux::Handle))) == NULL)
      return -ENOMEM;

    if ((d->dp = ::opendir(way.c_str())) == NULL)
      {
	res = -errno;
	free(d);
	return res;
      }

    d->offset = 0;
    d->entry = NULL;

    fi->fh = (unsigned long)d;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Readdir(const char*		path,
				      void*			buf,
				      ::fuse_fill_dir_t		filler,
				      off_t			offset,
				      struct ::fuse_file_info*	fi)
  {
    Crux::Handle*	d = (Crux::Handle*)fi->fh;

    (void)path;

    if (offset != d->offset)
      {
	::seekdir(d->dp, offset);

	d->entry = NULL;
	d->offset = offset;
      }

    while (1)
      {
	struct ::stat	st;
	off_t		nextoff;

	if (!d->entry)
	  {
	    if ((d->entry = ::readdir(d->dp)) == NULL)
	      break;
	  }

	::memset(&st, 0, sizeof(st));

	st.st_ino = d->entry->d_ino;
	st.st_mode = d->entry->d_type << 12;

	nextoff = ::telldir(d->dp);

	if (filler(buf, d->entry->d_name, &st, nextoff))
	  break;

	d->entry = NULL;
	d->offset = nextoff;
      }

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Releasedir(const char*		path,
					 struct ::fuse_file_info* fi)
  {
    Crux::Handle*	d = (Crux::Handle*)fi->fh;

    (void)path;

    ::closedir(d->dp);

    ::free(d);

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Mkdir(const char*			path,
				    mode_t			mode)
  {
    elle::String	way(Crux::Mirror + path);

    if (::mkdir(way.c_str(), mode) == -1)
      return -errno;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Rmdir(const char*			path)
  {
    elle::String	way(Crux::Mirror + path);

    if (::rmdir(way.c_str()) == -1)
      return -errno;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Access(const char*		path,
				     int			mask)
  {
    elle::String	way(Crux::Mirror + path);

    if (::access(way.c_str(), mask) == -1)
      return -errno;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Chmod(const char*			path,
				    mode_t			mode)
  {
    elle::String	way(Crux::Mirror + path);

    if (::chmod(way.c_str(), mode) == -1)
      return -errno;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Chown(const char*			path,
				    uid_t			uid,
				    gid_t			gid)
  {
    elle::String	way(Crux::Mirror + path);

    if (::lchown(way.c_str(), uid, gid) == -1)
      return -errno;

    return 0;
  }

#ifdef HAVE_SETXATTR

  ///
  /// XXX
  ///
  int			Crux::Setxattr(const char*		path,
				       const char*		name,
				       const char*		value,
				       size_t			size,
				       int			flags)
  {
    elle::String	way(Crux::Mirror + path);

    if (::lsetxattr(way.c_str(), name, value, size, flags) == -1)
      return -errno;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Getxattr(const char*		path,
				       const char*		name,
				       char*			value,
				       size_t			size)
  {
    elle::String	way(Crux::Mirror + path);
    int			res;

    if ((res = ::lgetxattr(way.c_str(), name, value, size)) == -1)
      return -errno;

    return res;
  }

  ///
  /// XXX
  ///
  int			Crux::Listxattr(const char*		path,
					char*			list,
					size_t			size)
  {
    elle::String	way(Crux::Mirror + path);
    int			res;

    if ((res = ::llistxattr(way.c_str(), list, size)) == -1)
      return -errno;

    return res;
  }

  ///
  /// XXX
  ///
  int			Crux::Removexattr(const char*		path,
					  const char*		name)
  {
    elle::String	way(Crux::Mirror + path);
    int			res;

    if (::lremovexattr(way.c_str(), name) == -1)
      return -errno;

    return 0;
  }

#endif

  ///
  /// XXX
  ///
  int			Crux::Symlink(const char*		to,
				      const char*		from)
  {
    elle::String	way(Crux::Mirror + from);

    if (::symlink(to, way.c_str()) == -1)
      return -errno;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Readlink(const char*		path,
				       char*			buf,
				       size_t			size)
  {
    elle::String	way(Crux::Mirror + path);
    int			res;

    if ((res = ::readlink(way.c_str(), buf, size - 1)) == -1)
      return -errno;

    buf[res] = '\0';

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Create(const char*		path,
				     mode_t			mode,
				     struct ::fuse_file_info*	fi)
  {
    elle::String	way(Crux::Mirror + path);
    int			fd;

    if ((fd = ::open(way.c_str(), fi->flags, mode)) == -1)
      return -errno;

    fi->fh = fd;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Open(const char*			path,
				   struct ::fuse_file_info*	fi)
  {
    elle::String	way(Crux::Mirror + path);
    int			fd;

    if ((fd = ::open(way.c_str(), fi->flags)) == -1)
      return -errno;

    fi->fh = fd;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Write(const char*			path,
				    const char*			buf,
				    size_t			size,
				    off_t			offset,
				    struct ::fuse_file_info*	fi)
  {
    int			res;

    (void)path;

    if ((res = ::pwrite(fi->fh, buf, size, offset)) == -1)
      res = -errno;

    return res;
  }

  ///
  /// XXX
  ///
  int			Crux::Read(const char*			path,
				   char*			buf,
				   size_t			size,
				   off_t			offset,
				   struct ::fuse_file_info*	fi)
  {
    int			res;

    (void)path;

    if ((res = ::pread(fi->fh, buf, size, offset)) == -1)
      res = -errno;

    return res;
  }

  ///
  /// XXX
  ///
  int			Crux::Truncate(const char*		path,
				       off_t			size)
  {
    elle::String	way(Crux::Mirror + path);

    if (::truncate(way.c_str(), size) == -1)
      return -errno;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Ftruncate(const char*		path,
					off_t			size,
					struct ::fuse_file_info* fi)
  {
    (void)path;

    if (::ftruncate(fi->fh, size) == -1)
      return -errno;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Release(const char*		path,
				      struct ::fuse_file_info*	fi)
  {
    (void)path;

    ::close(fi->fh);

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Rename(const char*		from,
				     const char*		to)
  {
    elle::String	f(Crux::Mirror + from);
    elle::String	t(Crux::Mirror + to);

    if (::rename(f.c_str(), t.c_str()) == -1)
      return -errno;

    return 0;
  }

  ///
  /// XXX
  ///
  int			Crux::Unlink(const char*		path)
  {
    elle::String	way(Crux::Mirror + path);

    if (::unlink(way.c_str()) == -1)
      return -errno;

    return 0;
  }

  ///
  /// this method sets up the crux.
  ///
  elle::Status		Crux::Setup(const elle::String&		mirror)
  {
    enter();

    // set the mirror path.
    if (mirror.empty() == false)
      Crux::Mirror = mirror + "/";

    //
    // initialize the FUSE operations.
    //
    {
      // set all the pointers to zero.
      ::memset(&Crux::Operations, 0x0, sizeof (::fuse_operations));

      // Crux::Operations.statfs: not supported
      Crux::Operations.getattr = Crux::Getattr;
      Crux::Operations.fgetattr = Crux::Fgetattr;
      Crux::Operations.utimens = Crux::Utimens;

      Crux::Operations.opendir = Crux::Opendir;
      Crux::Operations.readdir = Crux::Readdir;
      Crux::Operations.releasedir = Crux::Releasedir;
      Crux::Operations.mkdir = Crux::Mkdir;
      Crux::Operations.rmdir = Crux::Rmdir;

      Crux::Operations.access = Crux::Access;
      Crux::Operations.chmod = Crux::Chmod;
      Crux::Operations.chown = Crux::Chown;

#ifdef HAVE_SETXATTR
      Crux::Operations.setxattr = Crux::Setxattr;
      Crux::Operations.getxattr = Crux::Getxattr;
      Crux::Operations.listxattr = Crux::Listxattr;
      Crux::Operations.removexattr = Crux::Removexattr;
#endif

      // Crux::Operations.link: not supported
      Crux::Operations.readlink = Crux::Readlink;
      Crux::Operations.symlink = Crux::Symlink;

      Crux::Operations.create = Crux::Create;
      // Crux::Operations.mknod: not supported
      Crux::Operations.open = Crux::Open;
      Crux::Operations.write = Crux::Write;
      Crux::Operations.read = Crux::Read;
      Crux::Operations.truncate = Crux::Truncate;
      Crux::Operations.ftruncate = Crux::Ftruncate;
      Crux::Operations.release = Crux::Release;

      Crux::Operations.rename = Crux::Rename;
      Crux::Operations.unlink = Crux::Unlink;

      Crux::Operations.flag_nullpath_ok = 1;
    }

    leave();
  }

}

  /* XXX[are these necessary? we will find out]
  ///
  /// XXX
  ///
  int			Crux::Lock(const char*			path,
				   struct ::fuse_file_info*	fi,
				   int				cmd,
				   struct ::flock*		lock)
  {
    (void)path;

    return ::ulockmgr_op(fi->fh,
			 cmd,
			 lock,
			 &fi->lock_owner,
			 sizeof(fi->lock_owner));
  }

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;

	if (S_ISFIFO(mode))
		res = mkfifo(path, mode);
	else
		res = mknod(path, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}




static int xmp_link(const char *from, const char *to)
{
	int res;

	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}


static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	int res;

	res = statvfs(path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_flush(const char *path, struct fuse_file_info *fi)
{
	int res;

	(void) path;
	// This is called from every close on an open file, so call the
	// close on the underlying filesystem.	But since flush may be
	// called multiple times for an open file, this must not really
	// close the file.  This is important if used on a network
	// filesystem like NFS which flush the data/metadata on close()
	res = close(dup(fi->fh));
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	int res;
	(void) path;

#ifndef HAVE_FDATASYNC
	(void) isdatasync;
#else
	if (isdatasync)
		res = fdatasync(fi->fh);
	else
#endif
		res = fsync(fi->fh);
	if (res == -1)
		return -errno;

	return 0;
}
  */
