/*
 * ---------- header ----------------------------------------------------------
 *
 * project       infinit
 *
 * license       infinit (c)
 *
 * file          /home/mycure/infinit/pig/pig.c
 *
 * created       julien quintard   [mon oct 22 12:26:58 2007]
 * updated       julien quintard   [thu jul 23 00:45:36 2009]
 */

/**
 * \file pig.c
 *
 * PIG main file. Implementation of the PIG interface.
 */

/*
 * ---------- includes --------------------------------------------------------
 */

#include "pig.hh"

#include <elle/Elle.hh>
#include <etoile/components/Components.hh>

#include <set>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace elle;
using namespace etoile::components;

//
// XXX
//
// o changer la taille des clefs
// o changer le facteur de replication
// o grouper les PKBs pour eviter la generation de clefs a chaque fois
//
// XXX
//

//
// ---------- cache -----------------------------------------------------------
//

std::set<const char*>	cache;

void			cache_put(const char*			path)
{
  cache.insert(path);
}

bool			cache_get(const char*			path)
{
  std::set<const char*>::iterator	iterator;

  iterator = cache.find(path);

  if (iterator == cache.end())
    return (false);

  return (true);
}

void			cache_pop(const char*			path)
{
  cache.erase(path);
}

//
// ---------- etoile ----------------------------------------------------------
//

int			etoile_new(const char*			path)
{
  char			name[4096];
  int			fd;

  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  sprintf(name, "%s.io", path);

  if ((fd = open(name, O_CREAT | O_EXCL | O_WRONLY, 0600)) == -1)
    return (-errno);

  KeyPair		owner;

  if (owner.Generate() == StatusError)
    return (-1);

  File			file;

  if (file.Create(owner) == StatusError)
    return (-1);

  if (file.Seal() == StatusError)
    return (-1);

  Archive		archive;

  if (archive.Create() == StatusError)
    return (-1);

  if (archive.Serialize(file) == StatusError)
    return (-1);

  if (write(fd, archive.contents, archive.size) == -1)
    return (-errno);

  if (close(fd) == -1)
    return (-errno);

  cache_put(name);

  return (0);
}

//
// XXX[the protocol here is complex, contacting several nodes etc.]
//
int			etoile_delete(const char*		path)
{
  char			name[4096];

  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  sprintf(name, "%s.io", path);

  if (unlink(name) != 0)
    return (-1);

  cache_pop(name);

  return (0);
}

int			etoile_retrieve(const char*		path,
					unsigned int		n)
{
  struct stat		stat;

  printf("XXX[%s(%s, %u)]\n", __PRETTY_FUNCTION__, path, n);

  switch (n)
    {
    case 1:
      {
	if (lstat(path, &stat) == -1)
	  return (-errno);

	usleep((unsigned int)(2000 * ((float)stat.st_size / 1024.0)));

	break;
      }
    default:
      {
	usleep(n * 21000);

	break;
      }
    }

  return (0);
}

int			etoile_access(const char*		path)
{
  char			name[4096];
  struct stat		stat;
  int			fd;

  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  sprintf(name, "%s.io", path);

  // ignore paths that are not related to the test such as /data etc.
  if (access(name, O_RDONLY) == -1)
    return (0);

  // if already in cache, needless to verify it.
  if (cache_get(name) == true)
    return (0);

  // retrieve it first.
  if (etoile_retrieve(name, 7) != 0)
    return (-1);

  if ((fd = open(name, O_RDONLY)) == -1)
    return (-errno);

  if (lstat(name, &stat) == -1)
    return (-errno);

  Region		region;

  if (region.Prepare(stat.st_size) == StatusError)
    return (-1);

  region.size = stat.st_size;

  if (read(fd, region.contents, region.size) == -1)
    return (-errno);

  if (close(fd) == -1)
    return (-errno);

  if (region.Detach() == StatusError)
    return (-1);

  Archive		archive;

  if (archive.Prepare(region) == StatusError)
    return (-1);

  File			file;

  if (archive.Extract(file) == StatusError)
    return (-1);

  if (file.Validate(file.address) != StatusTrue)
    return (-1);

  cache_put(name);

  return (0);
}

/*
 * ---------- functions -------------------------------------------------------
 */

int			infinit_getattr(const char*		path,
					struct stat*		stat)
{
  printf("XXX[%s(%s)]\n", __PRETTY_FUNCTION__, path);

  if (lstat(path, stat) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_readlink(const char*		path,
					 char*			buffer,
					 size_t			size)
{
  int			length;

  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  if ((length = readlink(path, buffer, size - 1)) == -1)
    return (-errno);

  buffer[length] = '\0';

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);

    if (etoile_retrieve(path, 1) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_mknod(const char*		path,
				       mode_t			mode,
				       dev_t			dev)
{
  int			fd;

  printf("XXX[%s(%s, %u, %u)]\n", __PRETTY_FUNCTION__, path, mode, dev);

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

  // XXX
  {
    if (etoile_new(path) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_mkdir(const char*		path,
				       mode_t			mode)
{
  printf("XXX[%s(%s, %u)]\n", __PRETTY_FUNCTION__, path, mode);

  if (mkdir(path, mode) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_new(path) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_unlink(const char*		path)
{
  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  if (unlink(path) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_delete(path) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_rmdir(const char*		path)
{
  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  if (rmdir(path) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_delete(path) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_symlink(const char*		to,
					 const char*		from)
{
  printf("XXX[%s(%s, %s)]\n", __PRETTY_FUNCTION__, to, from);

  if (symlink(to, from) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_new(from) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_rename(const char*		from,
					const char*		to)
{
  printf("XXX[%s(%s, %s)]\n", __PRETTY_FUNCTION__, from, to);

  if (rename(from, to) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_new(to) != 0)
      return (-1);

    if (etoile_delete(from) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_link(const char*		to,
				      const char*		from)
{
  printf("XXX[%s(%s, %s)]\n", __PRETTY_FUNCTION__, to, from);

  if (link(to, from) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_new(from) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_chmod(const char*		path,
				       mode_t			mode)
{
  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  if (chmod(path, mode) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_chown(const char*		path,
				       uid_t			uid,
				       gid_t			gid)
{
  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  if (lchown(path, uid, gid) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_truncate(const char*		path,
					  off_t			size)
{
  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  if (truncate(path, size) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_open(const char*		path,
				      struct fuse_file_info*	info)
{
  int			fd;

  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  if ((fd = open(path, info->flags)) == -1)
    return (-errno);

  close(fd);

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_read(const char*		path,
				      char*			buffer,
				      size_t			size,
				      off_t			offset,
				      struct fuse_file_info*	info)
{
  int			length;
  int			fd;

  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  if ((fd = open(path, O_RDONLY)) == -1)
    return (-errno);

  if ((length = pread(fd, buffer, size, offset)) == -1)
    {
      close(fd);

      return (-errno);
    }

  close(fd);

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);

    if (etoile_retrieve(path, 1) != 0)
      return (-1);
  }
  // XXX

  return (length);
}

int			infinit_write(const char*		path,
				       const char*		buffer,
				       size_t			size,
				       off_t			offset,
				       struct fuse_file_info*	info)
{
  int			length;
  int			fd;

  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  if ((fd = open(path, O_WRONLY)) == -1)
    return (-errno);

  if ((length = pwrite(fd, buffer, size, offset)) == -1)
    {
      close(fd);

      return (-errno);
    }

  close(fd);

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);
  }
  // XXX

  return (length);
}

int			infinit_statfs(const char*		path,
					struct statvfs*		stat)
{
  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  if (statvfs(path, stat) == -1)
    return (-errno);

  return (0);
}

int			infinit_release(const char*		path,
					 struct fuse_file_info*	info)
{
  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

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
  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

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
  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  if (lsetxattr(path, name, value, size, flags) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_getxattr(const char*		path,
					  const char*		name,
					  char*			value,
					  size_t		size)
{
  int			attribute;

  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  if ((attribute = lgetxattr(path, name, value, size)) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);
  }
  // XXX

  return (attribute);
}

int			infinit_listxattr(const char*		path,
					   char*		list,
					   size_t		size)
{
  int			attributes;

  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  if ((attributes = llistxattr(path, list, size)) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);
  }
  // XXX

  return (attributes);
}

int			infinit_removexattr(const char*	path,
					     const char*	name)
{
  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  if (lremovexattr(path, name) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);
  }
  // XXX

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

  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

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

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_access(const char*		path,
					int			mask)
{
  printf("XXX[%s(%s, %u)]\n", __PRETTY_FUNCTION__, path, mask);

  if (access(path, mask) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

int			infinit_utimens(const char*		path,
					 const struct timespec	ts[2])
{
  struct timeval	tv[2];

  printf("XXX[%s]\n", __PRETTY_FUNCTION__);

  tv[0].tv_sec = ts[0].tv_sec;
  tv[0].tv_usec = ts[0].tv_nsec / 1000;
  tv[1].tv_sec = ts[1].tv_sec;
  tv[1].tv_usec = ts[1].tv_nsec / 1000;

  if (utimes(path, tv) == -1)
    return (-errno);

  // XXX
  {
    if (etoile_access(path) != 0)
      return (-1);
  }
  // XXX

  return (0);
}

/*
 * ---------- table -----------------------------------------------------------
 */

// [XXX] retoucher ce tableau
struct fuse_operations infinit_operations;

void			initialize()
{
  infinit_operations.getattr = infinit_getattr;
  infinit_operations.access = infinit_access;
  infinit_operations.readlink = infinit_readlink;
  infinit_operations.readdir = infinit_readdir;
  infinit_operations.mknod = infinit_mknod;
  infinit_operations.mkdir = infinit_mkdir;
  infinit_operations.symlink = infinit_symlink;
  infinit_operations.unlink = infinit_unlink;
  infinit_operations.rmdir = infinit_rmdir;
  infinit_operations.rename = infinit_rename;
  infinit_operations.link = infinit_link;
  infinit_operations.chmod = infinit_chmod;
  infinit_operations.chown = infinit_chown;
  infinit_operations.truncate = infinit_truncate;
  infinit_operations.utimens = infinit_utimens;
  infinit_operations.open = infinit_open;
  infinit_operations.read = infinit_read;
  infinit_operations.write = infinit_write;
  infinit_operations.statfs = infinit_statfs;
  infinit_operations.release = infinit_release;
  infinit_operations.fsync = infinit_fsync;
  infinit_operations.setxattr = infinit_setxattr;
  infinit_operations.getxattr = infinit_getxattr;
  infinit_operations.listxattr = infinit_listxattr;
  infinit_operations.removexattr = infinit_removexattr;
}

/*
 * ---------- entry point -----------------------------------------------------
 */

int			main(int				argc,
			     char*				argv[])
{
  umask(0);

  // [XXX] handle options and build a specific argv

  initialize();

  Cryptography::Initialize();

  if (fuse_main(argc, argv, &infinit_operations, NULL) != 0)
    return (1);

  return (0);
}
