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
 * ---------- includes --------------------------------------------------------
 */

#include <elle/Elle.hh>
#include <etoile/components/Components.hh>

using namespace elle;
using namespace etoile::components;

//
// ---------- macros ----------------------------------------------------------
//

#define HOLE_OBJECT		0
#define HOLE_DATA		1
#define HOLE_IDENTIFIERS	2

#define HOLE_IDENTIFIER_OBJECT	"objects"
#define HOLE_IDENTIFIER_DATA	"data"

//
// ---------- globals ---------------------------------------------------------
//

char*			g_device = NULL;

//
// ---------- hole ------------------------------------------------------------
//

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

/*
 * ---------- entry point -----------------------------------------------------
 */

void			usage()
{
  printf("usage: mkfs.infinit [path-to-device]\n");
}

int			mkfs(int				argc,
			     char*				argv[])
{
  if (argc != 2)
    {
      usage();
      exit(EXIT_FAILURE);
    }

  g_device = argv[1];

  /*
   * create and initialize the device.
   */
  {
    char		path[4096];

    mkdir(g_device, 0700);

    sprintf(path, "%s/objects", g_device);
    mkdir(path, 0700);

    sprintf(path, "%s/data", g_device);
    mkdir(path, 0700);
  }

  /*
   * initialize the libraries.
   */
  {
    Cryptography::Initialize();
  }

  /*
   * create the directory.
   */
  KeyPair		owner;
  Directory		directory;
  Catalog		catalog;
  {
    // generate owner keypair
    if (owner.Generate() == StatusError)
      return (-1);

    // create directory object.
    if (directory.Create(owner) == StatusError)
      return (-1);

    if (directory.Seal() == StatusError)
      return (-1);

    // create directory data: empty for now because the . and .. entries
    // are not recorded internally.
    if (catalog.Seal() == StatusError)
      return (-1);

    // update directory object.
    if (directory.Update(owner.k, catalog.address) == StatusError)
      return (-1);

    if (directory.Seal() == StatusError)
      return (-1);
  }

  /*
   * store the whole.
   */
  {
    Archive		a_directory;
    char*		i_directory;

    // serialize the directory object.
    if (a_directory.Create() == StatusError)
      return (-1);

    if (a_directory.Serialize(directory) == StatusError)
      return (-1);

    // retrieve the storage identifier and store the archive.
    if ((i_directory = hole_identifier(HOLE_IDENTIFIER_OBJECT, "/")) == NULL)
      return (-errno);

    if (hole_put(i_directory,
		 (void*)a_directory.contents,
		 a_directory.size) != 0)
      return (-errno);

    free(i_directory);

    // serialize the catalog.
    Archive		a_catalog;
    char*		i_catalog;

    // serialize the catalog.
    if (a_catalog.Create() == StatusError)
      return (-1);

    if (a_catalog.Serialize(catalog) == StatusError)
      return (-1);

    // retrieve the storage identifier and store the archive.
    if ((i_catalog = hole_identifier(HOLE_IDENTIFIER_DATA, "/")) == NULL)
      return (-errno);

    if (hole_put(i_catalog,
		 (void*)a_catalog.contents,
		 a_catalog.size) != 0)
      return (-errno);

    free(i_catalog);
  }

  return (0);
}

int			main(int				argc,
			     char*				argv[])
{
  mkfs(argc, argv);

  expose();
}
