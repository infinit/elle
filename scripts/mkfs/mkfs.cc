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
#include <etoile/core/Core.hh>

using namespace elle;

using namespace etoile::core;

//
// ---------- globals ---------------------------------------------------------
//

char*			g_device = ".device";

//
// ---------- hole ------------------------------------------------------------
//

int			hole_put(const char*			id,
				 void*				contents,
				 unsigned int			size)
{
  char			path[4096];
  int			fd;

  printf("XXX[%s(%s, 0x%x, %u)]\n",
	 __FUNCTION__, id, contents, size);

  sprintf(path, "%s/%s", g_device, id);

  if ((fd = open(path, O_CREAT | O_EXCL | O_WRONLY, 0600)) == -1)
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

  /*
   * create the device directory.
   */
  {
    mkdir(g_device, 0700);
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
    String		identity;

    Archive		a_directory;

    // serialize the directory object.
    if (a_directory.Create() == StatusError)
      return (-1);

    if (a_directory.Serialize(directory) == StatusError)
      return (-1);

    // store the directory.
    if (directory.address.Identify(identity) == StatusError)
      return (-1);

    if (hole_put(identity.c_str(),
		 (void*)a_directory.contents,
		 a_directory.size) != 0)
      return (-errno);

    // serialize the catalog.
    Archive		a_catalog;

    // serialize the catalog.
    if (a_catalog.Create() == StatusError)
      return (-1);

    if (a_catalog.Serialize(catalog) == StatusError)
      return (-1);

    // store the catalog.
    if (catalog.address.Identify(identity) == StatusError)
      return (-1);

    if (hole_put(identity.c_str(),
		 (void*)a_catalog.contents,
		 a_catalog.size) != 0)
      return (-errno);
  }

  /*
   * XXX[hack for bootstrapping the system]
   *
   * store the directory into .root
   */
  {
    Archive		a_address;
    String		identity;

    // create the archive.
    if (a_address.Create() == StatusError)
      return (-1);

    // serialize the directory address.
    if (a_address.Serialize(directory.address) == StatusError)
      return (-1);

    // transform the address into a string and store that in a file.
    if (hole_put(".root",
		 (void*)a_address.contents,
		 a_address.size) != 0)
      return (-errno);
  }

  return (0);
}

int			main(int				argc,
			     char*				argv[])
{
  int			status;

  status = mkfs(argc, argv);

  expose();

  return (status);
}
