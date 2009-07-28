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
 * - creer une classe Time et faire des conversion avec time_t (slongword):
 *   nombres de secondes depuis Epoch.
 */

/*
 * ---------- includes --------------------------------------------------------
 */

#include "pig.hh"

#include <elle/Elle.hh>
#include <etoile/components/Components.hh>

using namespace elle;
using namespace etoile::components;

//
// ---------- globals ---------------------------------------------------------
//

char*			g_device = ".device";

//
// ---------- macros ----------------------------------------------------------
//

#define HOLE_OBJECT		0
#define HOLE_DATA		1
#define HOLE_IDENTIFIERS	2

#define HOLE_IDENTIFIER_OBJECT	"objects"
#define HOLE_IDENTIFIER_DATA	"data"

#define HOLE_TYPE_FILE		1
#define HOLE_TYPE_DIRECTORY	2

//
// ---------- macro-functions -------------------------------------------------
//

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

char*			hole_identifier(const char*		store,
					const char*		path)
{
  char*			id;
  unsigned int		i;

  printf("XXX[%s(%s, %s)]\n",
	 __FUNCTION__, store, path);

  if ((id = (char*)malloc(strlen(g_device) + 1 +
			  strlen(store) + 1 +
			  strlen(path) + 1)) == NULL)
    return (NULL);

  sprintf(id, "%s/%s/%s", g_device, store, path);

  for (i = strlen(g_device) + 1 + strlen(store) + 1; id[i] != '\0'; i++)
    if (id[i] == '/')
      id[i] = '#';

  return (id);
}

//
// ---------- pig -------------------------------------------------------------
//

int			pig_object(const char*			id,
				   Object&			object)
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
  Object		object;
  char*			id;

  printf("XXX[%s(%s, 0x%x)]\n",
	 __FUNCTION__, path, stat);

  memset(stat, 0x0, sizeof(struct stat));

  if ((id = hole_identifier(HOLE_IDENTIFIER_OBJECT, path)) == NULL)
    return (-errno);

  if (pig_object(id, object) != 0)
    error(ENOENT);

  switch (object.meta.status.type)
    {
    case Object::TypeFile:
      {
	stat->st_mode = S_IFREG | 0600;
	break;
      }
    case Object::TypeDirectory:
      {
	stat->st_mode = S_IFDIR | 0700;
	break;
      }
    }

  stat->st_uid = getuid();
  stat->st_gid = getgid();

  stat->st_size = object.meta.status.size;
  stat->st_atime = time(NULL);
  stat->st_mtime = time(NULL);
  stat->st_ctime = time(NULL);

  return (0);
}

int			fuse_mknod(const char*			path,
				   mode_t			mode,
				   dev_t			dev)
{
  char*			id[HOLE_IDENTIFIERS];

  printf("XXX[%s(%s, %u, %u)]\n",
	 __FUNCTION__, path, mode, dev);

  // handle unimplemented features.
  if (!S_ISREG(mode))
    {
      printf("[fatal] unimplemented feature\n");
      while (1);
    }

  if ((id[HOLE_OBJECT] = hole_identifier(HOLE_IDENTIFIER_OBJECT,
					 path)) == NULL)
    return (-errno);

  if ((id[HOLE_DATA] = hole_identifier(HOLE_IDENTIFIER_DATA,
				       path)) == NULL)
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

  if (hole_put(id[HOLE_OBJECT], (void*)archive.contents, archive.size) != 0)
    return (-errno);

  if (hole_put(id[HOLE_DATA], NULL, 0) != 0)
    return (-errno);

  free(id[HOLE_OBJECT]);
  free(id[HOLE_DATA]);

  return (0);
}

int			fuse_unlink(const char*			path)
{
  char*			id;

  printf("XXX[%s()]\n",
	 __FUNCTION__);

  if ((id = hole_identifier(HOLE_IDENTIFIER_OBJECT,
			    path)) == NULL)
    return (-errno);

  // XXX extract the data blocks and remove them as well.

  if (hole_destroy(id) != 0)
    return (-errno);

  return (0);
}

int			fuse_open(const char*			path,
				  struct fuse_file_info*	info)
{
  printf("XXX[%s(%s, 0x%x)]\n",
	 __FUNCTION__, path, info);

  // XXX

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

  // XXX

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

  // XXX

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
  char*			id;
  Object		object;

  printf("XXX[%s(%s, %u)]\n",
	 __FUNCTION__, path, mask);

  if ((id = hole_identifier(HOLE_IDENTIFIER_OBJECT, path)) == NULL)
    return (-errno);

  if (pig_object(id, object) != 0)
    error(ENOENT);

  // XXX check the permission.
  printf("%d %d %d %d\n", R_OK, W_OK, X_OK, F_OK);

  return (0);
}

int			fuse_chmod(const char*			path,
				   mode_t			mode)
{
  printf("XXX[%s(%s, 0x%x)]\n",
	 __FUNCTION__, path, mode);

  // XXX to do

  return (-1);
}

int			fuse_utimens(const char*		path,
				     const struct timespec	ts[2])
{
  printf("XXX[%s(%s, 'ts')]\n",
	 __FUNCTION__, path);

  // XXX to do

  return (0);
}

int			fuse_readdir(const char*		path,
				     void*			buffer,
				     fuse_fill_dir_t		filler,
				     off_t			offset,
				     struct fuse_file_info*	info)
{
  printf("XXX[%s(%s, 0x%x, 0x%x, %u, 0x%x)]\n",
	 __FUNCTION__, path, buffer, filler, offset, info);

  char*			c;

  if ((c = hole_identifier(HOLE_IDENTIFIER_DATA, path)) == NULL)
    return (-errno);

  void*			contents;
  unsigned int		size;

  if (hole_get(c, &contents, &size) != 0)
    error(-1);

  Region		region;

  if (region.Acquire((Byte*)contents, size) == StatusError)
    return (-1);

  if (region.Detach() == StatusError)
    return (-1);

  Archive		archive;

  if (archive.Prepare(region) == StatusError)
    return (-1);

  Catalog		catalog;

  if (archive.Extract(catalog) == StatusError)
    return (-1);

  // XXX[weird validation since in this implementation, the address (id)
  //     is not the one used internally]
  if (catalog.Validate(catalog.address) != StatusTrue)
    return (-1);

  std::list<Catalog::Entry*>::iterator iterator;
  Natural32		i;

  // return the static entries . and ..
  filler(buffer, ".", NULL, 1);
  filler(buffer, "..", NULL, 0);

  // XXX[this implementation does not handle offsets hence could be very
  //     slow for big directories. might be, to study!]
  /*
  for (iterator = catalog.entries.begin(), i = 3;
       iterator != catalog.entries.end();
       iterator++, i++)
    {
      Catalog::Entry*       entry = *iterator;

      if (filler(buffer, entry->name.c_str(), NULL, 0) != 0)
      //break;
    }
  */

  return (0);
}

/*
 * ---------- table -----------------------------------------------------------
 */

struct fuse_operations fuse_operations;

/*
 * ---------- entry point -----------------------------------------------------
 */

int			main(int				argc,
			     char*				argv[])
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
  fuse_operations.readdir = fuse_readdir;

  Cryptography::Initialize();

  if (fuse_main(argc, argv, &fuse_operations, NULL) != 0)
    return (1);

  return (0);
}
