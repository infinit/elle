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

/*
 * XXX[TODO]
 *
 * - remove core/Object et utiliser ~XXX(); new (this) XXX(); a la place
 * - surtout ca rend ambigu l'utilisation de Object dans ce code.
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

#define error(_errno_)							\
  {									\
    errno = _errno_;							\
    return (-errno);							\
  }

//
// ---------- hole ------------------------------------------------------------
//

int			hole_put(const char*			id,
				 void*				contents,
				 unsigned int			size)
{
  int			fd;

  printf("XXX[%s(%s, 0x%x, %u)]\n",
	 __FUNCTION__, id, contents, size);

  if ((fd = open(id, O_CREAT | O_EXCL | O_WRONLY, 0600)) == -1)
    return (-errno);

  if (write(fd, contents, size) == -1)
    return (-errno);

  if (close(fd) == -1)
    return (-errno);

  return (0);
}

int			hole_get(const char*			id,
				 void**				contents,
				 unsigned int*			size)
{
  int			fd;
  struct stat		stat;

  printf("XXX[%s(%s, 0x%x, 0x%x)]\n",
	 __FUNCTION__, id, contents, size);

  if (lstat(id, &stat) == -1)
    return (-errno);

  *size = stat.st_size;

  if ((*contents = (void*)malloc(*size)) == NULL)
    return (-errno);

  if ((fd = open(id, O_RDONLY)) == -1)
    return (-errno);

  if (read(fd, *contents, *size) == -1)
    return (-errno);

  if (close(fd) == -1)
    return (-errno);

  return (0);
}

int			hole_destroy(const char*		id)
{
  printf("XXX[%s(%s)]\n",
	 __FUNCTION__, id);

  unlink(id);
}

//
// ---------- pig -------------------------------------------------------------
//

#define PIG_OBJECT		0
#define PIG_DATA		1
#define PIG_IDENTIFIERS		2

#define PIG_IDENTIFIER_OBJECT	"/tmp/.objects"
#define PIG_IDENTIFIER_DATA	"/tmp/.data"

#define PIG_TYPE_FILE		1
#define PIG_TYPE_DIRECTORY	2

char*			pig_identifier(const char*		store,
				       const char*		path)
{
  char*			id;
  unsigned int		i;

  printf("XXX[%s(%s, %s)]\n",
	 __FUNCTION__, store, path);

  if ((id = (char*)malloc(strlen(store) + 1 + strlen(path) + 1)) == NULL)
    return (NULL);

  sprintf(id, "%s/%s", store, path);

  for (i = strlen(store) + 1; id[i] != '\0'; i++)
    if (id[i] == '/')
      id[i] = '#';

  return (id);
}

int			pig_object(const char*			id,
				   etoile::components::Object&	object)
{
  void*			contents;
  unsigned int		size;

  printf("XXX[%s(%s, 0x%x)]\n",
	 __FUNCTION__, id, &object);

  if (hole_get(id, &contents, &size) != 0)
    return (-errno);

  Region		region;

  if (region.Acquire((Byte*)contents, size) == StatusError)
    return (-1);

  if (region.Detach() == StatusError)
    return (-1);

  Archive		archive;

  if (archive.Prepare(region) == StatusError)
    return (-1);

  if (archive.Extract(object) == StatusError)
    return (-1);

  // XXX[weird validation since in this implementation, the address (id)
  //     is not the one used internally]
  if (object.Validate(object.address) != StatusTrue)
    return (-1);

  return (0);
}

//
// ---------- fuse ------------------------------------------------------------
//

int			fuse_getattr(const char*		path,
				     struct stat*		stat)
{
  etoile::components::Object	object;
  char*			id;

  printf("XXX[%s(%s, 0x%x)]\n",
	 __FUNCTION__, path, stat);

  if (strcmp(path, "/") == 0)
    {
      // XXX[hack: instead create an object through mkfs.infinit]
      stat->st_mode = S_IFDIR | 0700;
      stat->st_nlink = 0;
      stat->st_uid = getuid();
      stat->st_gid = getgid();
      stat->st_size = 0;
      stat->st_blksize = 0;
      stat->st_atime = time(NULL);
      stat->st_mtime = time(NULL);
      stat->st_ctime = time(NULL);

      return (0);
    }

  if ((id = pig_identifier(PIG_IDENTIFIER_OBJECT, path)) == NULL)
    return (-errno);

  if (pig_object(id, object) != 0)
    error(ENOENT);

  switch (object.meta.status.type)
    {
    case etoile::components::Object::TypeFile:
      {
	stat->st_mode = S_IFREG | 0600;
	break;
      }
    case etoile::components::Object::TypeDirectory:
      {
	stat->st_mode = S_IFDIR | 0700;
	break;
      }
    }

  stat->st_nlink = 0;
  stat->st_uid = getuid();
  stat->st_gid = getgid();
  stat->st_size = 0;
  stat->st_blksize = 0;
  stat->st_atime = time(NULL);
  stat->st_mtime = time(NULL);
  stat->st_ctime = time(NULL);

  return (0);
}

int			fuse_mknod(const char*			path,
				   mode_t			mode,
				   dev_t			dev)
{
  char*			id[PIG_IDENTIFIERS];

  printf("XXX[%s(%s, %u, %u)]\n",
	 __FUNCTION__, path, mode, dev);

  // handle unimplemented features.
  if (!S_ISREG(mode))
    {
      printf("[fatal] unimplemented feature\n");
      while (1);
    }

  if ((id[PIG_OBJECT] = pig_identifier(PIG_IDENTIFIER_OBJECT, path)) == NULL)
    return (-errno);

  if ((id[PIG_DATA] = pig_identifier(PIG_IDENTIFIER_DATA, path)) == NULL)
    return (-errno);

  KeyPair		owner;

  if (owner.Generate() == StatusError)
    return (-1);

  Archive		archive;

  if (archive.Create() == StatusError)
    return (-1);

  File			file;

  if (file.Create(owner) == StatusError)
    return (-1);

  // XXX[handle mode for the permissions!]

  if (file.Seal() == StatusError)
    return (-1);

  if (archive.Serialize(file) == StatusError)
    return (-1);

  if (hole_put(id[PIG_OBJECT], (void*)archive.contents, archive.size) != 0)
    return (-errno);

  if (hole_put(id[PIG_DATA], NULL, 0) != 0)
    return (-errno);

  free(id[PIG_OBJECT]);
  free(id[PIG_DATA]);

  return (0);
}

int			fuse_unlink(const char*			path)
{
  printf("XXX[%s()]\n",
	 __FUNCTION__);

  if (unlink(path) == -1)
    return (-errno);

  return (0);
}

int			fuse_open(const char*			path,
				  struct fuse_file_info*	info)
{
  int			fd;

  printf("XXX[%s(%s, 0x%x)]\n",
	 __FUNCTION__, path, info);

  if ((fd = open(path, info->flags)) == -1)
    return (-errno);

  close(fd);

  return (0);
}

int			fuse_read(const char*			path,
				  char*				buffer,
				  size_t			size,
				  off_t				offset,
				  struct fuse_file_info*	info)
{
  int			length;
  int			fd;

  printf("XXX[%s(%s, 0x%x, %u, %u, 0x%x)]\n",
	 __FUNCTION__, path, buffer, size, offset, info);

  if ((fd = open(path, O_RDONLY)) == -1)
    return (-errno);

  if ((length = pread(fd, buffer, size, offset)) == -1)
    {
      close(fd);

      return (-errno);
    }

  close(fd);

  return (length);
}

int			fuse_write(const char*			path,
				   const char*			buffer,
				   size_t			size,
				   off_t			offset,
				   struct fuse_file_info*	info)
{
  int			length;
  int			fd;

  printf("XXX[%s(%s, 0x%x, %u, %u, 0x%x)]\n",
	 __FUNCTION__, path, buffer, size, offset, info);

  if ((fd = open(path, O_WRONLY)) == -1)
    return (-errno);

  if ((length = pwrite(fd, buffer, size, offset)) == -1)
    {
      close(fd);

      return (-errno);
    }

  close(fd);

  return (length);
}

int			fuse_release(const char*		path,
				     struct fuse_file_info*	info)
{
  printf("XXX[%s(%s, 0x%x)]\n",
	 __FUNCTION__, path, info);

  /*
   * just a stub.
   *
   * this method is optional and can safely be left unimplemented
   */

  return (0);
}

int			fuse_access(const char*			path,
				    int				mask)
{
  printf("XXX[%s(%s, %u)]\n",
	 __FUNCTION__, path, mask);

  if (access(path, mask) == -1)
    return (-errno);

  return (0);
}

int			fuse_chmod(const char*			path,
				   mode_t			mode)
{
  printf("XXX[%s(%s, 0x%x)]\n",
	 __FUNCTION__, path, mode);

  if (chmod(path, mode) == -1)
    return (-errno);

  return (0);
}

int			fuse_utimens(const char*		path,
				     const struct timespec	ts[2])
{
  printf("XXX[%s(%s, 'ts')]\n",
	 __FUNCTION__, path);

  return (0);
}

/*
 * ---------- table -----------------------------------------------------------
 */

struct fuse_operations fuse_operations;

void			initialize()
{
  fuse_operations.getattr = fuse_getattr;
  fuse_operations.access = fuse_access;
  fuse_operations.mknod = fuse_mknod;
  fuse_operations.unlink = fuse_unlink;
  fuse_operations.open = fuse_open;
  fuse_operations.read = fuse_read;
  fuse_operations.write = fuse_write;
  fuse_operations.release = fuse_release;
  fuse_operations.utimens = fuse_utimens;
}

/*
 * ---------- entry point -----------------------------------------------------
 */

int			main(int				argc,
			     char*				argv[])
{
  umask(0);

  initialize();

  Cryptography::Initialize();

  if (fuse_main(argc, argv, &fuse_operations, NULL) != 0)
    return (1);

  return (0);
}
