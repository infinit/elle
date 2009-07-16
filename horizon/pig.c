/*
 * ---------- header ----------------------------------------------------------
 *
 * project       infinit
 *
 * license       infinit (c)
 *
 * file          /home/mycure/pers...s/ucam/phd/development/sources/pig/pig.c
 *
 * created       julien quintard   [mon oct 22 12:26:58 2007]
 * updated       julien quintard   [fri oct 26 22:02:47 2007]
 */

/**
 * \file pig.c
 *
 * PIG main file. Impletation of the PIG interface.
 */

/*
 * ---------- includes --------------------------------------------------------
 */

#include "pig.h"

/*
 * ---------- functions -------------------------------------------------------
 */

/**
 * Get attribute for file pointed by path.
 *
 * \param path	path to file
 * \param stat  structure receiving file attributes
 *
 * \return 0 for SUCCEED, errno otherwise
 */
int			infinit_getattr(const char*		path,
					 struct stat*		stat)
{
  if (lstat(path, stat) == -1)
    return (-errno);

  return (0);
}

int			infinit_readlink(const char*		path,
					  char*			buffer,
					  size_t		size)
{
  int			length;

  if ((length = readlink(path, buffer, size - 1)) == -1)
    return (-errno);

  buffer[length] = '\0';

  return (0);
}

int			infinit_mknod(const char*		path,
				       mode_t			mode,
				       dev_t			dev)
{
  int			fd;

  if (S_ISREG(mode))
    {
      if ((fd = open(path, O_CREAT | O_EXCL | O_WRONLY, mode)) == -1)
	return (-errno);

      if (close(fd) == -1)
	return (-errno);
    }
  else if (S_ISFIFO(mode))
    {
      if (mkfifo(path, mode) == -1)
	return (-errno);
    }
  else
    {
      if (mknod(path, mode, dev) == -1)
        return (-errno);
    }

  return (0);
}

int			infinit_mkdir(const char*		path,
				       mode_t			mode)
{
  if (mkdir(path, mode) == -1)
    return (-errno);

  return (0);
}

int			infinit_unlink(const char*		path)
{
  if (unlink(path) == -1)
    return (-errno);

  return (0);
}

int			infinit_rmdir(const char*		path)
{
  if (rmdir(path) == -1)
    return (-errno);

  return (0);
}

int			infinit_symlink(const char*		from,
					 const char*		to)
{
  if (symlink(from, to) == -1)
    return (-errno);

  return (0);
}

int			infinit_rename(const char*		from,
					const char*		to)
{
  if (rename(from, to) == -1)
    return (-errno);

  return (0);
}

int			infinit_link(const char*		from,
				      const char*		to)
{
  if (link(from, to) == -1)
    return (-errno);

  return (0);
}

int			infinit_chmod(const char*		path,
				       mode_t			mode)
{
  if (chmod(path, mode) == -1)
    return (-errno);

  return (0);
}

int			infinit_chown(const char*		path,
				       uid_t			uid,
				       gid_t			gid)
{
  if (lchown(path, uid, gid) == -1)
    return (-errno);

  return (0);
}

int			infinit_truncate(const char*		path,
					  off_t			size)
{
  if (truncate(path, size) == -1)
    return (-errno);

  return (0);
}

int			infinit_utime(const char*		path,
				       struct utimbuf*		buffer)
{
  if (utime(path, buffer) == -1)
    return (-errno);

  return (0);
}

int			infinit_open(const char*		path,
				      struct fuse_file_info*	info)
{
  int			fd;

  if ((fd = open(path, info->flags)) == -1)
    return (-errno);

  close(fd);

  return (0);
}

int			infinit_read(const char*		path,
				      char*			buffer,
				      size_t			size,
				      off_t			offset,
				      struct fuse_file_info*	info)
{
  int			fd;

  if ((fd = open(path, O_RDONLY)) == -1)
    return (-errno);

  if (pread(fd, buffer, size, offset) == -1)
    {
      close(fd);

      return (-errno);
    }

  close(fd);

  return (0);
}

int			infinit_write(const char*		path,
				       const char*		buffer,
				       size_t			size,
				       off_t			offset,
				       struct fuse_file_info*	info)
{
  int			fd;

  if ((fd = open(path, O_WRONLY)) == -1)
    return (-errno);

  if (pwrite(fd, buffer, size, offset) == -1)
    {
      close(fd);

      return (-errno);
    }

  close(fd);

  return (0);
}

int			infinit_statfs(const char*		path,
					struct statvfs*		stat)
{
  if (statvfs(path, stat) == -1)
    return (-errno);

  return (0);
}

int			infinit_flush(const char*		path,
				       struct fuse_file_info*	info)
{
  /*
   * just a stub.
   *
   * this method is optional and can safely be left unimplemented
   */

  return (0);
}

int			infinit_release(const char*		path,
					 struct fuse_file_info*	info)
{
  /*
   * just a stub.
   *
   * this method is optional and can safely be left unimplemented
   */

  return (0);
}

int			infinit_fsync(const char*		path,
				       int			synchronised,
				       struct fuse_file_info*	info)
{
  /*
   * just a stub.
   *
   * this method is optional and can safely be left unimplemented
   */

  return (0);
}

int			infinit_setxattr(const char*		path,
					  const char*		name,
					  const char*		value,
					  size_t		size,
					  int			flags)
{
  if (lsetxattr(path, name, value, size, flags) == -1)
    return (-errno);

  return (0);
}

int			infinit_getxattr(const char*		path,
					  const char*		name,
					  char*			value,
					  size_t		size)
{
  if (lgetxattr(path, name, value, size) == -1)
    return (-errno);

  return (0);
}

int			infinit_listxattr(const char*		path,
					   char*		list,
					   size_t		size)
{
  if (llistxattr(path, list, size) == -1)
    return (-errno);

  return (0);
}

int			infinit_removexattr(const char*	path,
					     const char*	name)
{
  if (lremovexattr(path, name) == -1)
    return (-errno);

  return (0);
}

int			infinit_opendir(const char*		path,
					 struct fuse_file_info*	info)
{
  DIR*			handle;

  if ((handle = opendir(path)) == NULL)
    return (-errno);

  closedir(handle);

  return (0);
}

int			infinit_readdir(const char*		path,
					 void*			buffer,
					 fuse_fill_dir_t	filler,
					 off_t			offset,
					 struct fuse_file_info*	info)
{
  DIR*			handle;
  struct dirent*	entry;

  if ((handle = opendir(path)) == NULL)
    return (-errno);

  while ((entry = readdir(handle)) != NULL)
    {
      struct stat	stat;

      memset(&stat, 0, sizeof(stat));

      stat.st_ino = entry->d_ino;
      stat.st_mode = entry->d_type << 12;

      if (filler(buffer, entry->d_name, &stat, 0))
	break;
    }

  closedir(handle);

  return (0);
}

int			infinit_releasedir(const char*		path,
					    struct fuse_file_info* info)
{
  /*
   * XXX
   */

  return (0);
}

int			infinit_fsyncdir(const char*		path,
					  int			synchronised,
					  struct fuse_file_info* info)
{
  /*
   * XXX
   */

  return (0);
}

void*		infinit_init(struct fuse_conn_info*	info)
{
  /*
   * XXX
   */

  return (NULL);
}

void		infinit_destroy(void*			data)
{
  /*
   * XXX
   */
}

int			infinit_access(const char*		path,
					int			mask)
{
  if (access(path, mask) == -1)
    return (-errno);

  return (0);
}

int			infinit_create(const char*		path,
					mode_t			mode,
					struct fuse_file_info*	info)
{
  /*
   * XXX
   */

  return (0);
}

int			infinit_ftruncate(const char*		path,
					   off_t		offset,
					   struct fuse_file_info* info)
{
  if (truncate(path, offset) == -1)
    return (-errno);

  return (0);
}

int			infinit_fgetattr(const char*		path,
					  struct stat*		stat,
					  struct fuse_file_info* info)
{
  /*
   * XXX
   */

  return (0);
}

int			infinit_lock(const char*		path,
				      struct fuse_file_info*	info,
				      int			cmd,
				      struct flock*		lock)
{
  /*
   * XXX
   */

  return (0);
}

int			infinit_utimens(const char*		path,
					 const struct timespec	ts[2])
{
  struct timeval	tv[2];

  tv[0].tv_sec = ts[0].tv_sec;
  tv[0].tv_usec = ts[0].tv_nsec / 1000;
  tv[1].tv_sec = ts[1].tv_sec;
  tv[1].tv_usec = ts[1].tv_nsec / 1000;

  if (utimes(path, tv) == -1)
    return (-errno);

  return (0);
}

int			infinit_bmap(const char*		path,
				      size_t			block,
				      uint64_t*			index)
{
  /*
   * XXX
   */

  return (0);
}

/*
 * ---------- table -----------------------------------------------------------
 */

// [XXX] retoucher ce tableau
static struct fuse_operations infinit_operations =
  {
    .getattr = infinit_getattr,
    .readlink = infinit_readlink,
    .mknod = infinit_mknod,
    .mkdir = infinit_mkdir,
    .unlink = infinit_unlink,
    .rmdir = infinit_rmdir,
    .symlink = infinit_symlink,
    .rename = infinit_rename,
    .link = infinit_link,
    .chmod = infinit_chmod,
    .chown = infinit_chown,
    .truncate = infinit_truncate,
    .utime = infinit_utime,
    .open = infinit_open,
    .read = infinit_read,
    .write = infinit_write,
    .statfs = infinit_statfs,
    .flush = infinit_flush,
    .release = infinit_release,
    .fsync = infinit_fsync,
    .setxattr = infinit_setxattr,
    .getxattr = infinit_getxattr,
    .listxattr = infinit_listxattr,
    .removexattr = infinit_removexattr,
    .opendir = infinit_opendir,
    .readdir = infinit_readdir,
    .releasedir = infinit_releasedir,
    .fsyncdir = infinit_fsyncdir,
    .init = infinit_init,
    .destroy = infinit_destroy,
    .access = infinit_access,
    .create = infinit_create,
    .ftruncate = infinit_ftruncate,
    .fgetattr = infinit_fgetattr,
    .lock = infinit_lock,
    .utimens = infinit_utimens,
    .bmap = infinit_bmap
};

/*
 * ---------- entry point -----------------------------------------------------
 */

int			 main(int				argc,
			      char*				argv[])
{
  umask(0);

  // [XXX] handle options and build a specific argv

  if (fuse_main(2, argv, &infinit_operations, NULL) != 0)
    return (1);

  return (0);
}
