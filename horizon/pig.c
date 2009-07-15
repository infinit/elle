/*
 * ---------- header ----------------------------------------------------------
 *
 * project       infinite
 *
 * license       GPL
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
int			infinite_getattr(const char*		path,
					 struct stat*		stat)
{
  if (lstat(path, stat) == -1)
    return (-errno);

  return (0);
}

int			infinite_readlink(const char*		path,
					  char*			buffer,
					  size_t		size)
{
  int			length;

  if ((length = readlink(path, buffer, size - 1)) == -1)
    return (-errno);

  buffer[length] = '\0';

  return (0);
}

int			infinite_mknod(const char*		path,
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

int			infinite_mkdir(const char*		path,
				       mode_t			mode)
{
  if (mkdir(path, mode) == -1)
    return (-errno);

  return (0);
}

int			infinite_unlink(const char*		path)
{
  if (unlink(path) == -1)
    return (-errno);

  return (0);
}

int			infinite_rmdir(const char*		path)
{
  if (rmdir(path) == -1)
    return (-errno);

  return (0);
}

int			infinite_symlink(const char*		from,
					 const char*		to)
{
  if (symlink(from, to) == -1)
    return (-errno);

  return (0);
}

int			infinite_rename(const char*		from,
					const char*		to)
{
  if (rename(from, to) == -1)
    return (-errno);

  return (0);
}

int			infinite_link(const char*		from,
				      const char*		to)
{
  if (link(from, to) == -1)
    return (-errno);

  return (0);
}

int			infinite_chmod(const char*		path,
				       mode_t			mode)
{
  if (chmod(path, mode) == -1)
    return (-errno);

  return (0);
}

int			infinite_chown(const char*		path,
				       uid_t			uid,
				       gid_t			gid)
{
  if (lchown(path, uid, gid) == -1)
    return (-errno);

  return (0);
}

int			infinite_truncate(const char*		path,
					  off_t			size)
{
  if (truncate(path, size) == -1)
    return (-errno);

  return (0);
}

int			infinite_utime(const char*		path,
				       struct utimbuf*		buffer)
{
  if (utime(path, buffer) == -1)
    return (-errno);

  return (0);
}

int			infinite_open(const char*		path,
				      struct fuse_file_info*	info)
{
  int			fd;

  if ((fd = open(path, info->flags)) == -1)
    return (-errno);

  close(fd);

  return (0);
}

int			infinite_read(const char*		path,
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

int			infinite_write(const char*		path,
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

int			infinite_statfs(const char*		path,
					struct statvfs*		stat)
{
  if (statvfs(path, stat) == -1)
    return (-errno);

  return (0);
}

int			infinite_flush(const char*		path,
				       struct fuse_file_info*	info)
{
  /*
   * just a stub.
   *
   * this method is optional and can safely be left unimplemented
   */

  return (0);
}

int			infinite_release(const char*		path,
					 struct fuse_file_info*	info)
{
  /*
   * just a stub.
   *
   * this method is optional and can safely be left unimplemented
   */

  return (0);
}

int			infinite_fsync(const char*		path,
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

int			infinite_setxattr(const char*		path,
					  const char*		name,
					  const char*		value,
					  size_t		size,
					  int			flags)
{
  if (lsetxattr(path, name, value, size, flags) == -1)
    return (-errno);

  return (0);
}

int			infinite_getxattr(const char*		path,
					  const char*		name,
					  char*			value,
					  size_t		size)
{
  if (lgetxattr(path, name, value, size) == -1)
    return (-errno);

  return (0);
}

int			infinite_listxattr(const char*		path,
					   char*		list,
					   size_t		size)
{
  if (llistxattr(path, list, size) == -1)
    return (-errno);

  return (0);
}

int			infinite_removexattr(const char*	path,
					     const char*	name)
{
  if (lremovexattr(path, name) == -1)
    return (-errno);

  return (0);
}

int			infinite_opendir(const char*		path,
					 struct fuse_file_info*	info)
{
  DIR*			handle;

  if ((handle = opendir(path)) == NULL)
    return (-errno);

  closedir(handle);

  return (0);
}

int			infinite_readdir(const char*		path,
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

int			infinite_releasedir(const char*		path,
					    struct fuse_file_info* info)
{
  /*
   * XXX
   */

  return (0);
}

int			infinite_fsyncdir(const char*		path,
					  int			synchronised,
					  struct fuse_file_info* info)
{
  /*
   * XXX
   */

  return (0);
}

void*		infinite_init(struct fuse_conn_info*	info)
{
  /*
   * XXX
   */

  return (NULL);
}

void		infinite_destroy(void*			data)
{
  /*
   * XXX
   */
}

int			infinite_access(const char*		path,
					int			mask)
{
  if (access(path, mask) == -1)
    return (-errno);

  return (0);
}

int			infinite_create(const char*		path,
					mode_t			mode,
					struct fuse_file_info*	info)
{
  /*
   * XXX
   */

  return (0);
}

int			infinite_ftruncate(const char*		path,
					   off_t		offset,
					   struct fuse_file_info* info)
{
  if (truncate(path, offset) == -1)
    return (-errno);

  return (0);
}

int			infinite_fgetattr(const char*		path,
					  struct stat*		stat,
					  struct fuse_file_info* info)
{
  /*
   * XXX
   */

  return (0);
}

int			infinite_lock(const char*		path,
				      struct fuse_file_info*	info,
				      int			cmd,
				      struct flock*		lock)
{
  /*
   * XXX
   */

  return (0);
}

int			infinite_utimens(const char*		path,
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

int			infinite_bmap(const char*		path,
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
static struct fuse_operations infinite_operations =
  {
    .getattr = infinite_getattr,
    .readlink = infinite_readlink,
    .mknod = infinite_mknod,
    .mkdir = infinite_mkdir,
    .unlink = infinite_unlink,
    .rmdir = infinite_rmdir,
    .symlink = infinite_symlink,
    .rename = infinite_rename,
    .link = infinite_link,
    .chmod = infinite_chmod,
    .chown = infinite_chown,
    .truncate = infinite_truncate,
    .utime = infinite_utime,
    .open = infinite_open,
    .read = infinite_read,
    .write = infinite_write,
    .statfs = infinite_statfs,
    .flush = infinite_flush,
    .release = infinite_release,
    .fsync = infinite_fsync,
    .setxattr = infinite_setxattr,
    .getxattr = infinite_getxattr,
    .listxattr = infinite_listxattr,
    .removexattr = infinite_removexattr,
    .opendir = infinite_opendir,
    .readdir = infinite_readdir,
    .releasedir = infinite_releasedir,
    .fsyncdir = infinite_fsyncdir,
    .init = infinite_init,
    .destroy = infinite_destroy,
    .access = infinite_access,
    .create = infinite_create,
    .ftruncate = infinite_ftruncate,
    .fgetattr = infinite_fgetattr,
    .lock = infinite_lock,
    .utimens = infinite_utimens,
    .bmap = infinite_bmap
};

/*
 * ---------- entry point -----------------------------------------------------
 */

int			 main(int				argc,
			      char*				argv[])
{
  umask(0);

  // [XXX] handle options and build a specific argv

  if (fuse_main(2, argv, &infinite_operations, NULL) != 0)
    return (1);

  return (0);
}
